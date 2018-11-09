# QrCode
Qr Code Generator
Uses SFML for preview and image creation.

# Usage
Write text into a file, then run QrCode from the command line
using one of the four error levels. Example:

    QRgen -f <inputfile>

will create a qr code
with error correction level 1, and a preview window will appear.

Alternatively, the message string can be passed via the command line:

    QRgen -m "This message is stored in the QR Code" -L 4

Press S to save.
