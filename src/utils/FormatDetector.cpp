#include "FormatDetector.h"
#include <QFile>
#include <QFileInfo>

ArchiveFormat FormatDetector::detectFormat(const QString &filePath) {
    ArchiveFormat format = detectBySignature(filePath);
    if (format != ArchiveFormat::Unknown) {
        return format;
    }
    return detectByExtension(filePath);
}

ArchiveFormat FormatDetector::detectBySignature(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return ArchiveFormat::Unknown;
    }
    
    QByteArray header = file.read(16);
    file.close();
    
    if (header.size() < 4) {
        return ArchiveFormat::Unknown;
    }
    
    // RAR signature: Rar!\x1a\x07\x00 or Rar!\x1a\x07\x01\x00
    if (header.startsWith("Rar!\x1a\x07")) {
        return ArchiveFormat::RAR;
    }
    
    // ZIP signature: PK\x03\x04 or PK\x05\x06 (empty) or PK\x07\x08 (spanned)
    if (header.startsWith("PK\x03\x04") || header.startsWith("PK\x05\x06") || header.startsWith("PK\x07\x08")) {
        return ArchiveFormat::ZIP;
    }
    
    // 7Z signature: 7z\xbc\xaf\x27\x1c
    if (header.startsWith("7z\xbc\xaf\x27\x1c")) {
        return ArchiveFormat::SevenZip;
    }
    
    // TAR: ustar or GNU tar magic
    if (header.size() >= 257 && (header.mid(257, 5) == "ustar" || header.mid(257, 6) == "ustar ")) {
        // Check extension for compression
        QString ext = QFileInfo(filePath).suffix().toLower();
        if (ext == "gz") {
            return ArchiveFormat::TarGz;
        } else if (ext == "bz2") {
            return ArchiveFormat::TarBz2;
        } else if (ext == "xz") {
            return ArchiveFormat::TarXz;
        }
        return ArchiveFormat::Tar;
    }
    
    return ArchiveFormat::Unknown;
}

ArchiveFormat FormatDetector::detectByExtension(const QString &filePath) {
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();
    QString baseName = info.completeBaseName();
    
    if (ext == "rar") {
        return ArchiveFormat::RAR;
    } else if (ext == "zip") {
        return ArchiveFormat::ZIP;
    } else if (ext == "7z") {
        return ArchiveFormat::SevenZip;
    } else if (ext == "tar") {
        return ArchiveFormat::Tar;
    } else if (ext == "gz") {
        if (baseName.endsWith(".tar")) {
            return ArchiveFormat::TarGz;
        }
    } else if (ext == "bz2") {
        if (baseName.endsWith(".tar")) {
            return ArchiveFormat::TarBz2;
        }
    } else if (ext == "xz") {
        if (baseName.endsWith(".tar")) {
            return ArchiveFormat::TarXz;
        }
    }
    
    return ArchiveFormat::Unknown;
}

QString FormatDetector::formatExtension(ArchiveFormat format) {
    switch (format) {
        case ArchiveFormat::RAR: return "rar";
        case ArchiveFormat::ZIP: return "zip";
        case ArchiveFormat::SevenZip: return "7z";
        case ArchiveFormat::Tar: return "tar";
        case ArchiveFormat::TarGz: return "tar.gz";
        case ArchiveFormat::TarBz2: return "tar.bz2";
        case ArchiveFormat::TarXz: return "tar.xz";
        default: return "";
    }
}

QString FormatDetector::formatName(ArchiveFormat format) {
    switch (format) {
        case ArchiveFormat::RAR: return "RAR";
        case ArchiveFormat::ZIP: return "ZIP";
        case ArchiveFormat::SevenZip: return "7Z";
        case ArchiveFormat::Tar: return "TAR";
        case ArchiveFormat::TarGz: return "TAR.GZ";
        case ArchiveFormat::TarBz2: return "TAR.BZ2";
        case ArchiveFormat::TarXz: return "TAR.XZ";
        default: return "Unknown";
    }
}
