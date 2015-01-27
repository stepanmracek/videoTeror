#include "vtsenseExtras/dircrawler.h"

QStringList VideoTeror::Extras::DirCrawler::recursiveSearch(const QDir &dir, const QStringList &filters)
{
    QStringList result;
    auto subdirs = dir.entryInfoList(QStringList(), QDir::Filters(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks),
                                     QDir::SortFlags(QDir::Name | QDir::IgnoreCase));
    foreach (const QFileInfo &subdir, subdirs)
    {
        result << recursiveSearch(QDir(subdir.absoluteFilePath()), filters);
    }

    auto files = dir.entryInfoList(filters, QDir::Files | QDir::System | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                                   QDir::Name | QDir::IgnoreCase);
    foreach (const QFileInfo &file, files)
    {
        result << file.absoluteFilePath();
    }

    return result;
}
