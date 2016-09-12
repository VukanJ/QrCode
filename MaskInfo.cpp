#include "MaskInfo.h"

MaskInfo::MaskInfo(qrInfo *inf) : info(inf) {}

void MaskInfo::fillData(std::vector<bool> &dataFinal, QR_Field &qr_field) {
  std::cout << "Writing QR Code ...\n";
  genFormatInfo();
  if (info->version + 1 >= 7)
    genVersionInfo();
  for (int i = 0; i < info->size; i++)
    qr_field.push_back(std::vector<QR_Module>(info->size, QR_Module()));
  bool up = true;
  int h = info->size - 1, w = info->size - 1;
  auto bit = dataFinal.begin();
  for (int i = 0; i < (info->size - 1) / 2; i++) {
    h = up ? info->size - 1 : 0;
    for (int j = 0; j < info->size; j++) {
      if (!qr_field[w][h].isLocked()) {
        qr_field[w][h].write(*(bit++), M_DATA);
      }
      if (!qr_field[w - 1][h].isLocked()) {
        qr_field[w - 1][h].write(*(bit++), M_DATA);
      }
      h = up ? h - 1 : h + 1;
    }
    up = up ? false : true;
    w = w == 8 ? w - 3 : w - 2;
  }
  if (info->version + 1 >= 7) { // Insert version information string
    int p = 17;
    for (int j = 0; j < 6; j++) {
      for (int i = 0; i < 3; i++, p--) {
        qr_field[info->size - 11 + i][j].unlock();
        qr_field[info->size - 11 + i][j].write_lock(versionInfo[p], M_FORMAT);
        qr_field[j][info->size - 11 + i].unlock();
        qr_field[j][info->size - 11 + i].write_lock(versionInfo[p], M_FORMAT);
      }
    }
  }
  dataFinal.clear();
}

void MaskInfo::applyMask(QR_Field &qr_field, int m) {
  std::function<bool(int, int)> mask_alg = nullptr;
  switch (m) {
  case 0:
    mask_alg = [](int i, int j) -> bool { return (i + j) % 2 == 0; };
    break;
  case 1:
    mask_alg = [](int i, int j) -> bool { return i % 2 == 0; };
    break;
  case 2:
    mask_alg = [](int i, int j) -> bool { return j % 3 == 0; };
    break;
  case 3:
    mask_alg = [](int i, int j) -> bool { return (i + j) % 3 == 0; };
    break;
  case 4:
    mask_alg = [](int i, int j) -> bool {
      return (int)(floor(i / 2) + floor(j / 3)) % 2 == 0;
    };
    break;
  case 5:
    mask_alg = [](int i, int j) -> bool {
      return (i * j) % 2 + (i * j) % 3 == 0;
    };
    break;
  case 6:
    mask_alg = [](int i, int j) -> bool {
      return ((i * j) % 2 + (i * j) % 3) % 2 == 0;
    };
    break;
  case 7:
    mask_alg = [](int i, int j) -> bool {
      return ((i + j) % 2 + (i * j) % 3) % 2 == 0;
    };
    break;
  default:
    std::cerr << "INVALID MASKING PATTERN SELECTED\n";
    exit(1);
    break;
  }
  for (int i = 0; i < info->size; i++)
    for (int j = 0; j < info->size; j++)
      qr_field[j][i].write(qr_field[j][i].state() != mask_alg(i, j), M_DATA);
  // Insert format information
  int p = formatInfo[m].size() - 1;
  for (int i = 0; i < 9; i++) {
    if (qr_field[8][i].type == M_FORMAT) {
      qr_field[8][i].unlock();
      qr_field[8][i].write_lock(formatInfo[m][p--], M_FORMAT);
    }
  }
  for (int i = 1; i < 9; i++) {
    if (qr_field[8 - i][8].type == M_FORMAT) {
      qr_field[8 - i][8].unlock();
      qr_field[8 - i][8].write_lock(formatInfo[m][p--], M_FORMAT);
    }
  }
  p = formatInfo[m].size() - 1;
  for (int i = 0; i < 8; i++) {
    if (qr_field[info->size - 1 - i][8].type == M_FORMAT) {
      qr_field[info->size - 1 - i][8].unlock();
      qr_field[info->size - 1 - i][8].write_lock(formatInfo[m][p--], M_FORMAT);
    }
  }
  for (int i = 0; i < 8; i++) {
    if (qr_field[8][info->size - 8 + i].type == M_FORMAT) {
      qr_field[8][info->size - 8 + i].unlock();
      qr_field[8][info->size - 8 + i].write_lock(formatInfo[m][p--], M_FORMAT);
    }
  }
}

void MaskInfo::maskEvaluate(QR_Field &qr_field) {
  bool readcolor;
  int count = 0;
  std::cout << "Applying masks ...\n";
  std::vector<int> penalty(8, 0);
  for (int M = 0; M < 8; M++) {
    applyMask(qr_field, M);
    // I. Look for subsequent, same coloured pixels in columns, then rows
    for (int r = 0, count = 0; r < 2; r++) {
      readcolor = qr_field[0][0].state();
      for (int i = 0; i < info->size; i++) {
        for (int j = 0, count = 0; j < info->size; j++) {
          if ((r == 0 ? qr_field[i][j] : qr_field[j][i]).state() == readcolor) {
            count++;
            if (count == 5)
              penalty[M] += 3;
            else if (count > 5)
              penalty[M]++;
          } else {
            count = 1;
            readcolor = readcolor ? false : true;
          }
        }
      }
    }
    // II. Look for (2x2) blocks of same colour
    for (int i = 0; i < info->size - 1; i++) {
      for (int j = 0; j < info->size - 1; j++) {
        readcolor = qr_field[i][j].state();
        if ((readcolor == qr_field[i + 1][j].state()) &&
            (readcolor == qr_field[i][j + 1].state()) &&
            (readcolor == qr_field[i + 1][j + 1].state()))
          penalty[M] += 3;
      }
    }
    // III. Look for 10111010000 and 00001011101
    const unsigned short pattern1 = 0x5D0, pattern2 = 0x5D;
    unsigned short ptr1 = 0x1 << 10, ptr2 = 0x1 << 10;
    for (int r = 0, count = 0; r < 2; r++) {
      for (int i = 0; i < info->size; i++) {
        for (int j = 0; j < info->size; j++) {
          readcolor = (r == 0 ? qr_field[i][j] : qr_field[j][i]).state();
          if (readcolor == static_cast<bool>(ptr1 & pattern1)) {
            ptr1 >>= 1;
            if (ptr1 == 0x0) {
              // printf("A[%d,%d] ",i,j);
              penalty[M] += 40;
              ptr1 = 0x1 << 10;
            }
          } else
            ptr1 = 0x1 << 10;
          if (readcolor == static_cast<bool>(ptr2 & pattern2)) {
            ptr2 >>= 1;
            if (ptr2 == 0x0) {
              penalty[M] += 40;
              ptr2 = 0x1 << 10;
            }
          } else
            ptr2 = 0x1 << 10;
        }
        ptr1 = ptr2 = 0x1 << 10;
      }
    }
    // IV. Evaluate White to Black ratio
    count = 0;
    float m1, m2;
    for (int i = 0; i < info->size; i++) {
      for (int j = 0; j < info->size; j++) {
        if (qr_field[i][j].state())
          count++;
      }
    }
    float black_percent = (float)count / (float)(info->size * info->size) * 10;
    if (black_percent - floor(black_percent) >= 0.5) {
      m1 = ceil(black_percent);
      m2 = m1 - 0.5;
      m1 *= 10;
      m2 *= 10;
    } else {
      m1 = floor(black_percent);
      m2 = m1 + 0.5;
      m1 *= 10;
      m2 *= 10;
    }
    penalty[M] += std::fmin(abs(50.f - m1) / 5.f, abs(50.f - m2)) * 10.f;
    applyMask(qr_field, M);
  }
  // std::cout << "Penalties: \n";
  // for (int p = 0; p < 8; p++)
  //	std::cout << p << ' ' << penalty[p] << std::endl;
  // Apply best Mask
  int best = penalty[0], ind = 0;
  for (int i = 1; i < 7; i++)
    if (penalty[i] < best) {
      best = penalty[i];
      ind = i;
    }
  std::cout << "Mask pattern " << ind << std::endl;
  applyMask(qr_field, ind);
}

void MaskInfo::genVersionInfo() {
  std::cout << "Generating version strings ...\n";
  // 8 Strings for 8 masking patterns
  const unsigned int GEN = 0x3E4A0; // 111110010010100000
  std::vector<bool> gen, temp, data;
  pushBits(GEN, gen, 18);
  temp.clear();
  data.clear();
  pushBits(info->version + 1, temp, 6);
  data = temp;
  pushBits(0x0, temp, 12);
  while (!temp[0])
    temp.erase(temp.begin());
  while (temp.size() != 12) {
    // XOR
    for (int k = 0; k < temp.size(); k++)
      temp[k] = (temp[k] != gen[k]);
    // Pad
    while (!temp[0] && temp.size() > 12)
      temp.erase(temp.begin());
  }
  // Assembly
  for (auto d : data)
    versionInfo.push_back(d);
  for (auto e : temp)
    versionInfo.push_back(e);
  DEBUG(for (auto f
             : versionInfo) std::cout
            << (f ? 1 : 0);
        std::cout << std::endl;)
}

void MaskInfo::genFormatInfo() {
  std::cout << "Generating format strings ...\n";
  // 8 Strings for 8 masking patterns
  const unsigned short XOR = 0x5412; // 101010000010010
  const unsigned short GEN = 0x5370; // 101001101110000
  std::vector<bool> gen, temp, data;
  pushBits(GEN, gen, 15);

  if (info->version < 0)
    return;
  byte ec = 0x0;
  switch (info->error_level) { // 1, 0, 3, 2
  case _L_:
    ec = 0x1;
    break;
  case _M_:
    ec = 0x0;
    break;
  case _Q_:
    ec = 0x3;
    break;
  case _H_:
    ec = 0x2;
    break;
  case _UNDEF_:
    throw std::string("Error: EC-Level undefined\n");
    break;
  }
  for (unsigned short i = 0; i < 8; i++) {
    temp.clear();
    data.clear();
    formatInfo.push_back(std::vector<bool>());
    pushBits(ec, temp, 2);
    pushBits(i, temp, 3);
    // Is data empty ? Can't perform division
    if (!std::any_of(temp.begin(), temp.end(), [](bool k) { return k; })) {
      temp.clear();
      pushBits(GEN ^ XOR, temp, 15);
      for (auto e : temp)
        formatInfo[i].push_back(e);
      continue;
    }
    data = temp;
    pushBits(0x0, temp, 10);
    while (!temp[0])
      temp.erase(temp.begin());
    while (temp.size() != 10) {
      // XOR
      for (int k = 0; k < temp.size(); k++)
        temp[k] = (temp[k] != gen[k]);
      // Pad
      while (!temp[0] && temp.size() > 10)
        temp.erase(temp.begin());
    }
    // Assemble and apply XOR mask
    for (auto d : data)
      formatInfo[i].push_back(d);
    for (auto e : temp)
      formatInfo[i].push_back(e);
    for (int k = 0; k < formatInfo[i].size(); k++)
      formatInfo[i][k] =
          (formatInfo[i][k] != static_cast<bool>((0x1 << (14 - k)) & XOR));
  }
  DEBUG(for (auto &F
             : formatInfo) {
    for (auto f : F) {
      std::cout << (f ? 1 : 0);
    }
    std::cout << std::endl;
  })
}
