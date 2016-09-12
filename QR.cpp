#include "QR.h"

QR::QR() {
  info.error_level = _UNDEF_;
  info.version = -1;
  encoder = new Encoder(&info);
  errorCoder = new EC(&info);
  maskinfo = new MaskInfo(&info);
  printer = new Printer(&info);
}

QR::~QR() {
  delete encoder;
  delete errorCoder;
  delete maskinfo;
  delete printer;
}

void QR::_reset_() {
  info = qrInfo{};
  encoder = new Encoder(&info);
  errorCoder = new EC(&info);
  maskinfo = new MaskInfo(&info);
  printer = new Printer(&info);
  qr_field = std::vector<std::vector<QR_Module>>{};
  qrCode = sf::Image{};
  dataFinal.clear();
}

sf::Image QR::create(const char *dataFile, Err_Level level) {
  std::cout << "\tGenerating QR-Code ...\n";
  info.error_level = level;
  std::cout << "Using error correction level ";
  switch (info.error_level) {
  case _L_:
    std::cout << "L (7";
    break;
  case _M_:
    std::cout << "M (15";
    break;
  case _Q_:
    std::cout << "Q (25";
    break;
  case _H_:
    std::cout << "H (30";
    break;
  default:
    std::cout << "Error: Unknown error level" << std::endl;
    throw;
  }
  std::cout << "% recovery) ...\n";
  encoder->getEncoding(dataFile);
  encoder->Encode(dataFinal);
  errorCoder->ErrorCorrection(dataFinal);
  printer->printAllPatterns(qr_field);
  maskinfo->fillData(dataFinal, qr_field);
  maskinfo->maskEvaluate(qr_field);
  // printer->printDebugFinal(qrCode,qr_field);
  printer->printFinal(qrCode, qr_field);
  return qrCode;
}

void QR::test(sf::RenderWindow &window) {
  window.setPosition(sf::Vector2i(0, 0));
  const std::wstring startMsg(L"QR ");
  std::wstring msg;
  std::wstring loop = (L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  auto it = loop.begin();
  for (int v = 0; v < 40; v++) {
    for (int Ec = 0; Ec < 4; Ec++) {
      _reset_();
      msg.clear();
      msg += startMsg;
      msg += L' ';
      msg += std::to_wstring(v + 1);
      switch (Ec) {
      case 0:
        msg += L"-L ";
        break;
      case 1:
        msg += L"-M ";
        break;
      case 2:
        msg += L"-Q ";
        break;
      case 3:
        msg += L"-H ";
        break;
      }
      it = loop.begin();
      while (msg.size() < versionSize[v * 4 + Ec][ALPHANUM]) {
        it = it + 1 == loop.end() ? loop.begin() : ++it;
        msg.push_back(*it);
      }
      encoder->read = msg;
      info.encoding = ALPHANUM;
      info.error_level = static_cast<Err_Level>(Ec);
      encoder->Encode(dataFinal);
      errorCoder->ErrorCorrection(dataFinal);
      printer->printAllPatterns(qr_field);
      maskinfo->fillData(dataFinal, qr_field);
      maskinfo->maskEvaluate(qr_field);
      printer->printFinal(qrCode, qr_field);
      //
      sf::Texture qrTex;
      qrTex.loadFromImage(qrCode);
      sf::Sprite qrSpr;
      qrSpr.setTexture(qrTex);
      // qrSpr.setScale(sf::Vector2f(5, 5));
      int size = qrSpr.getLocalBounds().width;
      // window.setSize(sf::Vector2u(size, size));
      // window.setSize(sf::Vector2u(5*(v * 4 + 29), 5*(v * 4 + 29)));
      window.draw(qrSpr);
      window.display();
    }
  }
}

int QR::getVersion() { return info.version; }