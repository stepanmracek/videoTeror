#ifndef DIRCRAWLER_H
#define DIRCRAWLER_H

#include <QStringList>
#include <QDir>

namespace VideoTeror
{
namespace Extras
{

class DirCrawler
{
public:
    static QStringList recursiveSearch(const QDir &dir, const QStringList &filters);
};

}
}

#endif // DIRCRAWLER_H
