/*
#
# Friction - https://friction.graphics
#
# Copyright (c) Friction contributors
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# See 'README.md' for more information.
#
*/

// Fork of enve - Copyright (C) 2016-2020 Maurycy Liebner

#include "shadervaluehandler.h"

ShaderValueHandler::ShaderValueHandler(const QString &name,
                                       const GLValueType type,
                                       const QString& script):
    fName(name), fScript(script), mType(type) {}

UniformSpecifier ShaderValueHandler::create(const GLint loc, QJSValue* getter) const {
    Q_ASSERT(loc >= 0);
    switch(mType) {
    case GLValueType::Float:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isNumber()) {
                gl->glUniform1f(loc, static_cast<GLfloat>(jsVal.toNumber()));
            } else RuntimeThrow("Invalid value. Expected float.");
        };
    case GLValueType::Vec2:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 2) RuntimeThrow("Invalid value. Expected vec2.");
                const qreal val0 = jsVal.property(0).toNumber();
                const qreal val1 = jsVal.property(1).toNumber();

                gl->glUniform2f(loc, static_cast<GLfloat>(val0),
                                static_cast<GLfloat>(val1));
            } else RuntimeThrow("Invalid value. Expected vec2.");
        };
    case GLValueType::Vec3:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 3) RuntimeThrow("Invalid value. Expected vec3.");
                const qreal val0 = jsVal.property(0).toNumber();
                const qreal val1 = jsVal.property(1).toNumber();
                const qreal val2 = jsVal.property(2).toNumber();

                gl->glUniform3f(loc, static_cast<GLfloat>(val0),
                                static_cast<GLfloat>(val1),
                                static_cast<GLfloat>(val2));
            } else RuntimeThrow("Invalid value. Expected vec3.");
        };
    case GLValueType::Vec4:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 4) RuntimeThrow("Invalid value. Expected vec4.");
                const qreal val0 = jsVal.property(0).toNumber();
                const qreal val1 = jsVal.property(1).toNumber();
                const qreal val2 = jsVal.property(2).toNumber();
                const qreal val3 = jsVal.property(3).toNumber();

                gl->glUniform4f(loc, static_cast<GLfloat>(val0),
                                static_cast<GLfloat>(val1),
                                static_cast<GLfloat>(val2),
                                static_cast<GLfloat>(val3));
            } else RuntimeThrow("Invalid value. Expected vec4.");
        };
    case GLValueType::Int:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isNumber()) {
                const int val = qRound(jsVal.toNumber());
                gl->glUniform1i(loc, static_cast<GLint>(val));
            } else RuntimeThrow("Invalid value. Expected int.");
        };
    case GLValueType::iVec2:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 2) RuntimeThrow("Invalid value. Expected ivec2.");
                const int val0 = qRound(jsVal.property(0).toNumber());
                const int val1 = qRound(jsVal.property(1).toNumber());

                gl->glUniform2i(loc, static_cast<GLint>(val0),
                                static_cast<GLint>(val1));
            } else RuntimeThrow("Invalid value. Expected ivec2.");
        };
    case GLValueType::iVec3:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 3) RuntimeThrow("Invalid value. Expected ivec3.");
                const int val0 = qRound(jsVal.property(0).toNumber());
                const int val1 = qRound(jsVal.property(1).toNumber());
                const int val2 = qRound(jsVal.property(2).toNumber());

                gl->glUniform3i(loc, static_cast<GLint>(val0),
                                static_cast<GLint>(val1),
                                static_cast<GLint>(val2));
            } else RuntimeThrow("Invalid value. Expected ivec3.");
        };
    case GLValueType::iVec4:
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 4) RuntimeThrow("Invalid value. Expected ivec4.");
                const int val0 = qRound(jsVal.property(0).toNumber());
                const int val1 = qRound(jsVal.property(1).toNumber());
                const int val2 = qRound(jsVal.property(2).toNumber());
                const int val3 = qRound(jsVal.property(3).toNumber());

                gl->glUniform4i(loc, static_cast<GLint>(val0),
                                static_cast<GLint>(val1),
                                static_cast<GLint>(val2),
                                static_cast<GLint>(val3));
            } else RuntimeThrow("Invalid value. Expected ivec4.");
        };
    default: RuntimeThrow("Unsupported type for " + fName);
    }

    if(mType == GLValueType::Float) {
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isNumber()) {
                gl->glUniform1f(loc, static_cast<GLfloat>(jsVal.toNumber()));
            } else RuntimeThrow("Invalid value. Expected float.");
        };
    } else if(mType == GLValueType::Int) {
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isNumber()) {
                gl->glUniform1i(loc, static_cast<GLint>(jsVal.toInt()));
            } else RuntimeThrow("Invalid value. Expected int.");
        };
    } else if(mType == GLValueType::Vec2) {
        return [loc, getter](QGL33 * const gl) {
            const QJSValue jsVal = getter->call();
            if(jsVal.isArray()) {
                const int len = jsVal.property("length").toInt();
                if(len != 2) RuntimeThrow("Invalid value. Expected vec2.");
                const qreal val0 = jsVal.property(0).toNumber();
                const qreal val1 = jsVal.property(1).toNumber();

                gl->glUniform2f(loc, static_cast<GLfloat>(val0),
                                static_cast<GLfloat>(val1));
            } else RuntimeThrow("Invalid value. Expected vec2.");
        };
    } else RuntimeThrow("Unsupported type for " + fName);
}
