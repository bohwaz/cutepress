#ifndef UTILS_H_
#define UTILS_H_

#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsItem>
#include <QSharedPointer>
#include <QTemporaryFile>

class QDir;
class QIODevice;

class QGraphicsLinearLayout;
class QGraphicsLayoutItem;

class Utils {
public:
    inline static bool smallScreen()
        {
        static SmallScreenState state(SmallScreenNA);
        if(state == SmallScreenNA)
            {
            QSize screen = QApplication::desktop()->screenGeometry().size();
            if(std::max(screen.width(), screen.height()) < 600)
                {
                state = SmallScreen;
                }
            else
                {
                state = BigScreen;
                }
            }
        return state;
        }
    
    // At least QGraphicsProxyWidgets need this hack to be focused
    static void ForceFocus(QGraphicsItem * item);
    
    static QSharedPointer<QTemporaryFile> getTempFileWithMinimumSize(quint64 minSizeInBytes);
    
    static quint64 getFreeSpaceInDir(QDir dir);
    
    static bool resizeImage(QString const & filename, QIODevice & output, const QSize& targetSize, const int angle);
    
    static int indexOfItem(QGraphicsLinearLayout * layout, QGraphicsLayoutItem * item);
    
    /**
     * Get the HTML to be used for rendering text.
     *
     * This is a very simplified version of the transformations that WordPress itself does,
     * so the result will never be a 100% match.
     *
     * WordPress is also customizeable, so any customizations affecting e.g. the 'the_content'
     * hook will break this.
     */
    static QString toDisplayHtml(QString const & string);
    
    /**
     * Open up the given QFile in the correct platform directory, retaining the given basename.
     */
    static bool openLocalDataFile (QFile & file, QFile::OpenMode mode);
    
private:
    enum SmallScreenState {
        SmallScreenNA  = -1,
        SmallScreen = true,
        BigScreen = false
    };
};

#endif /* UTILS_H_ */
