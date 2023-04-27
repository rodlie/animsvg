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

#include "svgexporthelpers.h"

#include "simplemath.h"

QString SvgExportHelpers::ptrToStr(const void* const ptr) {
    const auto size = QT_POINTER_SIZE * 2;
    return QString("0x%1").arg((quintptr)ptr, size, 16, QChar('0'));
}

void SvgExportHelpers::assignLoop(QDomElement& ele, const bool loop) {
    if(loop) ele.setAttribute("repeatCount", "indefinite");
    else ele.setAttribute("fill", "freeze");
}

sk_sp<SkData> asDataUri(SkImage* image,
                        SkEncodedImageFormat format = SkEncodedImageFormat::kPNG,
                        int quality = 100)
{
    switch (format) {
    case SkEncodedImageFormat::kBMP:
    case SkEncodedImageFormat::kGIF:
    case SkEncodedImageFormat::kICO:
    case SkEncodedImageFormat::kWBMP:
    case SkEncodedImageFormat::kPKM:
    case SkEncodedImageFormat::kKTX:
    case SkEncodedImageFormat::kASTC:
    case SkEncodedImageFormat::kDNG:
    case SkEncodedImageFormat::kHEIF:
        format = SkEncodedImageFormat::kPNG;
        quality = 100;
    default:;
    }

    sk_sp<SkData> imageData = image->encodeToData(format, quality);
    if (!imageData) {
        return nullptr;
    }

    const char* selectedPrefix = nullptr;
    size_t selectedPrefixLength = 0;

    const static char pngDataPrefix[] = "data:image/png;base64,";
    const static char jpgDataPrefix[] = "data:image/jpeg;base64,";
    const static char webpDataPrefix[] = "data:image/webp;base64,";

    switch (format) {
    case SkEncodedImageFormat::kJPEG:
        selectedPrefix = jpgDataPrefix;
        selectedPrefixLength = sizeof(jpgDataPrefix);
        break;
    case SkEncodedImageFormat::kWEBP:
        selectedPrefix = webpDataPrefix;
        selectedPrefixLength = sizeof(webpDataPrefix);
        break;
    default:
        selectedPrefix = pngDataPrefix;
        selectedPrefixLength = sizeof(pngDataPrefix);
    }

    size_t b64Size = SkBase64::Encode(imageData->data(), imageData->size(), nullptr);
    sk_sp<SkData> dataUri = SkData::MakeUninitialized(selectedPrefixLength + b64Size);
    char* dest = (char*)dataUri->writable_data();
    memcpy(dest, selectedPrefix, selectedPrefixLength);
    SkBase64::Encode(imageData->data(), imageData->size(), dest + selectedPrefixLength - 1);
    dest[dataUri->size() - 1] = 0;
    return dataUri;
}

void SvgExportHelpers::defImage(SvgExporter& exp,
                                const sk_sp<SkImage>& image,
                                const QString id)
{
    auto def = exp.createElement("image");
    def.setAttribute("id", id);
    def.setAttribute("x", 0);
    def.setAttribute("y", 0);
    def.setAttribute("width", image->width());
    def.setAttribute("height", image->height());
    const auto dataUri = asDataUri(image.get(),
                                   exp.fImageFormat,
                                   exp.fImageQuality);
    def.setAttribute("xlink:href", static_cast<const char*>(dataUri->data()));
    exp.addToDefs(def);
}

void SvgExportHelpers::assignVisibility(SvgExporter& exp,
                                        QDomElement& ele,
                                        const FrameRange& visRange) {
    if(visRange.inRange(exp.fAbsRange)) return;

    const qreal div = exp.fAbsRange.span() - 1;
    const qreal dur = div/exp.fFps;

    const qreal begin = (visRange.fMin - exp.fAbsRange.fMin)/div;
    const qreal end = qMin(1., (visRange.fMax - exp.fAbsRange.fMin + 1)/div);

    auto anim = exp.createElement("animate");
    anim.setAttribute("attributeName", "visibility");

    QString values;
    QString keyTimes;
    if(!isZero6Dec(begin)) {
        values += "hidden;";
        keyTimes += "0;";
    }
    values += "visible;hidden";
    keyTimes += QString::number(begin) + ";" + QString::number(end);
    if(!isOne6Dec(end)) {
        values += ";hidden";
        keyTimes += ";1";
    }
    anim.setAttribute("keyTimes", keyTimes);
    anim.setAttribute("values", values);

    anim.setAttribute("dur", QString::number(dur) + 's');

    SvgExportHelpers::assignLoop(anim, exp.fLoop);

    ele.setAttribute("visibility", "hidden");
    ele.appendChild(anim);
}
