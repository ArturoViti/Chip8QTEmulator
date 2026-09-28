#ifndef CHIP8QTEMULATOR_LOGHANDLER_H
#define CHIP8QTEMULATOR_LOGHANDLER_H

#include <QLoggingCategory>
#include <QFile>

namespace Log {
    inline Q_DECLARE_LOGGING_CATEGORY(Chip8Log)
    inline Q_LOGGING_CATEGORY(Chip8Log, "Chip8")

    inline Q_DECLARE_LOGGING_CATEGORY(EmulatorLog)
    inline Q_LOGGING_CATEGORY(EmulatorLog, "Emulator")

    inline void messageHandler( const QtMsgType type, const QMessageLogContext& context, const QString& msg ) {
        if ( QFile logFile("emulator.log"); logFile.open(QIODevice::WriteOnly | QIODevice::Append) )
            logFile.write(qUtf8Printable(qFormatLogMessage(type, context, msg) + "\n"));
    }

    const static std::string LOG_FORMAT = "%{time yyyy-MM-dd hh:mm:ss,zzz} [%{type}] %{category}: %{message}";
}


#endif //CHIP8QTEMULATOR_LOGHANDLER_H
