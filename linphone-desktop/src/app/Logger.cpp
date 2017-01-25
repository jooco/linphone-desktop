#include <bctoolbox/logging.h>
#include <linphone/linphonecore.h>
#include <QDateTime>

#include "Paths.hpp"

#include "Logger.hpp"

#ifdef __linux__
  #define BLUE "\x1B[1;34m"
  #define YELLOW "\x1B[1;33m"
  #define GREEN "\x1B[1;32m"
  #define PURPLE "\x1B[1;35m"
  #define RED "\x1B[1;31m"
  #define RESET "\x1B[0m"
#else
  #define BLUE ""
  #define YELLOW ""
  #define GREEN ""
  #define PURPLE ""
  #define RED ""
  #define RESET ""
#endif // ifdef __linux__

#define QT_DOMAIN "qt"

#define MAX_LOGS_COLLECTION_SIZE 104857600 /* 100MB. */

// =============================================================================

Logger *Logger::m_instance = nullptr;

// -----------------------------------------------------------------------------

static void qtLogger (QtMsgType type, const QMessageLogContext &context, const QString &msg) {
  const char *format;
  BctbxLogLevel level;

  if (type == QtDebugMsg) {
    format = GREEN "[%s][Debug]" PURPLE "%s" RESET "%s\n";
    level = BCTBX_LOG_DEBUG;
  } else if (type == QtInfoMsg) {
    format = BLUE "[%s][Info]" PURPLE "%s" RESET "%s\n";
    level = BCTBX_LOG_MESSAGE;
  } else if (type == QtWarningMsg) {
    format = RED "[%s][Warning]" PURPLE "%s" RESET "%s\n";
    level = BCTBX_LOG_WARNING;
  } else if (type == QtCriticalMsg) {
    format = RED "[%s][Critical]" PURPLE "%s" RESET "%s\n";
    level = BCTBX_LOG_ERROR;
  } else if (type == QtFatalMsg) {
    format = RED "[%s][Fatal]" PURPLE "%s" RESET "%s\n";
    level = BCTBX_LOG_FATAL;
  } else
    return;

  const char *context_str = "";

  #ifdef QT_MESSAGELOGCONTEXT
    QByteArray context_arr = QStringLiteral("%1:%2: ").arg(context.file).arg(context.line).toLocal8Bit();
    context_str = context_arr.constData();
  #endif // ifdef QT_MESSAGELOGCONTEXT

  QByteArray local_msg = msg.toLocal8Bit();
  QByteArray date_time = QDateTime::currentDateTime().toString("HH:mm:ss").toLocal8Bit();

  fprintf(stderr, format, date_time.constData(), context_str, local_msg.constData());
  bctbx_log(QT_DOMAIN, level, "QT: %s%s", context_str, local_msg.constData());

  if (type == QtFatalMsg)
    abort();
}

// -----------------------------------------------------------------------------

static void linphoneLogger (const char *domain, OrtpLogLevel type, const char *fmt, va_list args) {
  const char *format;

  if (type == ORTP_DEBUG)
    format = GREEN "[%s][Debug]" YELLOW "Core:%s: " RESET "%s\n";
  else if (type == ORTP_TRACE)
    format = BLUE "[%s][Trace]" YELLOW "Core:%s: " RESET "%s\n";
  else if (type == ORTP_MESSAGE)
    format = BLUE "[%s][Info]" YELLOW "Core:%s: " RESET "%s\n";
  else if (type == ORTP_WARNING)
    format = RED "[%s][Warning]" YELLOW "Core:%s: " RESET "%s\n";
  else if (type == ORTP_ERROR)
    format = RED "[%s][Critical]" YELLOW "Core:%s: " RESET "%s\n";
  else if (type == ORTP_FATAL)
    format = RED "[%s][Fatal]" YELLOW "Core:%s: " RESET "%s\n";
  else
    return;

  QByteArray date_time = QDateTime::currentDateTime().toString("HH:mm:ss").toLocal8Bit();
  char *msg = bctbx_strdup_vprintf(fmt, args);

  fprintf(stderr, format, date_time.constData(), domain, msg);

  bctbx_free(msg);

  if (type == ORTP_FATAL)
    abort();
}

// -----------------------------------------------------------------------------

void Logger::init () {
  if (m_instance)
    return;
  m_instance = new Logger();

  qInstallMessageHandler(qtLogger);

  linphone_core_set_log_level(ORTP_MESSAGE);
  linphone_core_set_log_handler(
    [](const char *domain, OrtpLogLevel type, const char *fmt, va_list args) {
      if (m_instance->m_display_core_logs)
        linphoneLogger(domain, type, fmt, args);
    }
  );

  linphone_core_set_log_collection_path(Paths::getLogsDirpath().c_str());
  linphone_core_set_log_collection_max_file_size(MAX_LOGS_COLLECTION_SIZE);
  linphone_core_enable_log_collection(LinphoneLogCollectionEnabled);
}