/*
 * Cli.hpp
 * Copyright (C) 2017  Belledonne Communications, Grenoble, France
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *  Created on: June 6, 2017
 *      Author: Nicolas Follet
 */

#ifndef CLI_H_
#define CLI_H_

#include <QHash>
#include <QObject>

// =============================================================================

class Cli : public QObject {
  Q_OBJECT;

  typedef void (*Function)(const QHash<QString, QString> &);

  enum ArgumentType {
    STRING
  };

  struct Argument {
    Argument (ArgumentType type = STRING, bool isOptional = false) {
      this->type = type;
      this->isOptional = isOptional;
    }

    ArgumentType type;
    bool isOptional;
  };

  class Command {
public:
    Command () = default;
    Command (const QString &functionName, const QString &description, Function function, const QHash<QString, Argument> &argsScheme);

    void execute (const QHash<QString, QString> &args);

    void executeUri(std::shared_ptr<linphone::Address> address);

    bool argNameExists (const QString &argName) {
      return mArgsScheme.contains(argName);
    }

private:
    QString mFunctionName;
    QString mDescription;
    Function mFunction = nullptr;
    QHash<QString, Argument> mArgsScheme;
  };

public:
  Cli (QObject *parent = Q_NULLPTR);
  ~Cli () = default;

  void executeCommand (const QString &command) noexcept;

private:
  void addCommand (const QString &functionName, const QString &description, Function function, const QHash<QString, Argument> &argsScheme = {}) noexcept;

  const QString parseFunctionName (const QString &command) noexcept;
  const QHash<QString, QString> parseArgs (const QString &command, const QString functionName, bool &soFarSoGood) noexcept;

  QHash<QString, Command> mCommands;

  static QRegExp mRegExpArgs;
  static QRegExp mRegExpFunctionName;
};

#endif // CLI_H_
