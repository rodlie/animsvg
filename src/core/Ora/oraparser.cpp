// enve - 2D animations software
// Copyright (C) 2016-2020 Maurycy Liebner

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "oraparser.h"

#include <QtXml/QDomDocument>

#include "zipfileloader.h"
#include "XML/xmlexporthelpers.h"

void parseOraElementAttributes(OraElement& oraEle,
                               const QDomElement &domEle) {
    const QString name = domEle.attribute("name");
    if(!name.isEmpty()) oraEle.fName = name;
    const QString xStr = domEle.attribute("x");
    if(!xStr.isEmpty()) {
        bool ok;
        const int x = xStr.toInt(&ok);
        if(ok) oraEle.fX = x;
    }
    const QString yStr = domEle.attribute("y");
    if(!yStr.isEmpty()) {
        bool ok;
        const int y = yStr.toInt(&ok);
        if(ok) oraEle.fY = y;
    }
    const QString opaStr = domEle.attribute("opacity");
    if(!opaStr.isEmpty()) {
        bool ok;
        const qreal opa = opaStr.toDouble(&ok);
        if(ok) oraEle.fOpacity = opa;
    }

    const QString visStr = domEle.attribute("visibility");
    if(visStr == "visible" || visStr == "hidden") {
        oraEle.fVisible = visStr == "visible";
    }

    const QString lockedStr = domEle.attribute("edit-locked");
    oraEle.fLocked = lockedStr == "true";

    const QString compOpStr = domEle.attribute("composite-op");
    oraEle.fBlend = XmlExportHelpers::stringToBlendMode(compOpStr);

    const QString selectedStr = domEle.attribute("selected");
    oraEle.fSelected = selectedStr == "true";
}

template <typename OraLayerPNG_XX,
          typename OraStack_XX = OraStack<OraLayerPNG_XX>>
std::shared_ptr<OraStack_XX> parseStack(const QDomElement &element);

template <typename OraLayerPNG_XX>
std::shared_ptr<OraLayer> parseLayer(const QDomElement &element) {
    std::shared_ptr<OraLayer> result;

    const auto src = element.attribute("src");
    const auto ext = src.right(3);
    if(ext == "svg") {
        result = std::make_shared<OraLayerSVG>();
    } else if(ext == "png") {
        result = std::make_shared<OraLayerPNG_XX>();
    }

    if(result) {
        result->fSource = src;
        parseOraElementAttributes(*result, element);
    }

    return result;
}

std::shared_ptr<OraText> parseText(const QDomElement &element) {
    auto result = std::make_shared<OraText>();
    parseOraElementAttributes(*result, element);
    result->fText = element.text();
    return result;
}

template <typename OraLayerPNG_XX,
          typename OraStack_XX = OraStack<OraLayerPNG_XX>>
void parseStackContent(OraStack_XX& stack, const QDomElement &element) {
    const auto childNodes = element.childNodes();
    for(int i = 0; i < childNodes.count(); i++) {
        const QDomNode iNode = childNodes.at(i);
        if(!iNode.isElement()) continue;
        const QDomElement iElement = iNode.toElement();
        const QString tagName = iElement.tagName();
        std::shared_ptr<OraElement> child;
        if(tagName == "stack") {
            child = parseStack<OraLayerPNG_XX>(iElement);
        } else if(tagName == "layer") {
            child = parseLayer<OraLayerPNG_XX>(iElement);
        } else if(tagName == "text") {
            child = parseText(iElement);
        } else RuntimeThrow("Unrecognized type " + tagName);
        stack.fChildren.push_back(std::move(child));
    }
}

template <typename OraLayerPNG_XX, typename OraStack_XX>
std::shared_ptr<OraStack_XX> parseStack(const QDomElement &element) {
    auto result = std::make_shared<OraStack_XX>();
    parseOraElementAttributes(*result, element);
    parseStackContent<OraLayerPNG_XX>(*result, element);
    return result;
}

template <typename OraLayerPNG_XX,
          typename OraImage_XX = OraImage<OraLayerPNG_XX>,
          typename OraStack_XX = OraStack<OraLayerPNG_XX>>
std::shared_ptr<OraImage_XX> parseStackXml(QIODevice* const src) {
    QDomDocument document;
    if(!document.setContent(src)) RuntimeThrow("Could not parse content");
    const QDomElement rootElement = document.firstChildElement("image");
    if(rootElement.isNull()) RuntimeThrow("Missing root (image) element");
    auto result = std::make_shared<OraImage_XX>();
    const QString wStr = rootElement.attribute("w");
    if(!wStr.isEmpty()) {
        bool ok;
        const int w = wStr.toInt(&ok);
        if(ok) result->fWidth = w;
    }
    const QString hStr = rootElement.attribute("h");
    if(!hStr.isEmpty()) {
        bool ok;
        const int h = hStr.toInt(&ok);
        if(ok) result->fHeight = h;
    }
    parseStackContent<OraLayerPNG_XX>(*result, rootElement);
    return result;
}

QSize parseOraSize(QIODevice* const src) {
    QDomDocument document;
    if(!document.setContent(src)) RuntimeThrow("Could not parse content");
    const QDomElement rootElement = document.firstChildElement("image");
    if(rootElement.isNull()) RuntimeThrow("Missing root (image) element");

    QSize result;

    const QString wStr = rootElement.attribute("w");
    if(!wStr.isEmpty()) {
        bool ok;
        const int w = wStr.toInt(&ok);
        if(ok) result.setWidth(w);
    }
    const QString hStr = rootElement.attribute("h");
    if(!hStr.isEmpty()) {
        bool ok;
        const int h = hStr.toInt(&ok);
        if(ok) result.setHeight(h);
    }
    return result;
}

template <typename OraLayerPNG_XX,
          typename OraImage_XX = OraImage<OraLayerPNG_XX>>
std::shared_ptr<OraImage_XX> readStackXml(ZipFileLoader& fileProcessor) {
    std::shared_ptr<OraImage_XX> result;
    fileProcessor.process("stack.xml", [&result](QIODevice* const src) {
        result = parseStackXml<OraLayerPNG_XX>(src);
    });
    return result;
}


void loadLayerSourcePNG(OraLayerPNG_Sk& layer, ZipFileLoader& fileProcessor) {
    fileProcessor.process(layer.fSource, [&](QIODevice* const src) {
        const QByteArray qData = src->readAll();
        const auto data = SkData::MakeWithoutCopy(qData.data(), qData.size());
        layer.fImage = SkImage::MakeFromEncoded(data);
    });
}

void loadLayerSourcePNG(OraLayerPNG_Qt& layer, ZipFileLoader& fileProcessor) {
    fileProcessor.process(layer.fSource, [&](QIODevice* const src) {
        const QByteArray qData = src->readAll();
        layer.fImage.loadFromData(qData);
    });
}

void loadLayerSourceSVG(OraLayerSVG& layer, ZipFileLoader& fileProcessor) {
    fileProcessor.process(layer.fSource, [&](QIODevice* const src) {
        layer.fDocument = src->readAll();
    });
}

template <typename OraLayerPNG_XX,
          typename OraStack_XX = OraStack<OraLayerPNG_XX>>
void loadLayerSourceFiles(OraStack_XX& stack, ZipFileLoader& fileProcessor) {
    for(const auto& child : stack.fChildren) {
        switch(child->fType) {
        case OraElementType::stack:
            loadLayerSourceFiles<OraLayerPNG_XX>(static_cast<OraStack_XX&>(*child),
                                                 fileProcessor);
            break;
        case OraElementType::layerPNG:
            loadLayerSourcePNG(static_cast<OraLayerPNG_XX&>(*child), fileProcessor);
            break;
        case OraElementType::layerSVG:
            loadLayerSourceSVG(static_cast<OraLayerSVG&>(*child), fileProcessor);
            break;
        default: break;
        }
    }
}

template <typename OraLayerPNG_XX>
std::shared_ptr<OraImage<OraLayerPNG_XX>> readOraFile(const QString &filename) {
    ZipFileLoader fileProcessor;
    fileProcessor.setZipPath(filename);

    const auto result = readStackXml<OraLayerPNG_XX>(fileProcessor);
    if(result) loadLayerSourceFiles<OraLayerPNG_XX>(*result, fileProcessor);
    return result;
}

std::shared_ptr<OraImage_Qt> ImportORA::readOraFileQImage(const QString &filename) {
    return readOraFile<OraLayerPNG_Qt>(filename);
}

std::shared_ptr<OraImage_Sk> ImportORA::readOraFileSkImage(const QString &filename) {
    return readOraFile<OraLayerPNG_Sk>(filename);
}

sk_sp<SkImage> ImportORA::loadContainedMerged(const QString &filename) {
    ZipFileLoader fileProcessor;
    fileProcessor.setZipPath(filename);

    sk_sp<SkImage> result;
    fileProcessor.process("mergedimage.png", [&](QIODevice* const src) {
        const QByteArray qData = src->readAll();
        const auto data = SkData::MakeWithoutCopy(qData.data(), qData.size());
        result = SkImage::MakeFromEncoded(data);
    });
    return result;
}
