#ifndef LOGGER
#define LOGGER

#include <iostream>
#include <streambuf>
#include <string>
#include <QTextEdit>

using namespace std;

namespace books_checker
{

class Logger: public std::basic_streambuf<char>
{
public:
    Logger(std::ostream& stream, QTextEdit* text_edit) : m_stream(stream)
    {
        log_window = text_edit;
        m_old_buf = stream.rdbuf();
        stream.rdbuf(this);
    }

    ~Logger()
    {
        m_stream.rdbuf(m_old_buf);
    }

    static void registerQDebugMessageHandler()
    {
        qInstallMessageHandler(myQDebugMessageHandler);
    }

    //printing string with current lime
    static ostream& Print()
    {
        cout << "" << currentDateTime() << " - ";
        return cout;
    }

private:
    static string currentDateTime()
    {
        time_t     now = time(0);
        struct tm  tstruct;
        char       buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%X", &tstruct);

        return buf;
    }

    static void myQDebugMessageHandler(QtMsgType, const QMessageLogContext&, const QString& msg)
    {
        std::cout << msg.toStdString().c_str();
    }

    std::ostream& m_stream;
    std::streambuf* m_old_buf;
    QTextEdit* log_window;

protected:

    //This is called when a std::endl has been inserted into the stream
    virtual int_type overflow(int_type v)
    {
        if (v == '\n')
        {
            log_window->append("");
        }
        return v;
    }


    virtual std::streamsize xsputn(const char* p, std::streamsize n)
    {
        QString str(p);
        if (str.contains("\n"))
        {
            QStringList strSplitted = str.split("\n");

            log_window->moveCursor(QTextCursor::End);
            log_window->insertPlainText(strSplitted.at(0));  //Index 0 is still on the same old line

            for (int i = 1; i < strSplitted.size(); i++)
            {
                log_window->append(strSplitted.at(i));
            }
        }
        else
        {
            log_window->moveCursor(QTextCursor::End);
            log_window->insertPlainText(str);
        }
        return n;
    }

};

}

#endif // LOGGER

