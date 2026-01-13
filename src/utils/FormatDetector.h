#ifndef FORMATDETECTOR_H
#define FORMATDETECTOR_H

#include <QString>

enum class ArchiveFormat {
    Unknown,
    RAR,
    ZIP,
    SevenZip,
    Tar,
    TarGz,
    TarBz2,
    TarXz
};

class FormatDetector {
public:
    static ArchiveFormat detectFormat(const QString &filePath);
    static QString formatExtension(ArchiveFormat format);
    static QString formatName(ArchiveFormat format);
    
private:
    static ArchiveFormat detectBySignature(const QString &filePath);
    static ArchiveFormat detectByExtension(const QString &filePath);
};

#endif // FORMATDETECTOR_H
