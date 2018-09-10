#include "QRCode.hpp"

#include <ImageReaderSource.h>

#include <iostream>
#include <vector>
#include <exception>
#include <fstream>

#include <zxing/common/Counted.h>
#include <zxing/Binarizer.h>
#include <zxing/MultiFormatReader.h>
#include <zxing/Result.h>
#include <zxing/ReaderException.h>
#include <zxing/common/GlobalHistogramBinarizer.h>
#include <zxing/common/HybridBinarizer.h>
#include <zxing/Exception.h>
#include <zxing/common/IllegalArgumentException.h>
#include <zxing/BinaryBitmap.h>
#include <zxing/DecodeHints.h>

#include <zxing/qrcode/QRCodeReader.h>
#include <zxing/multi/qrcode/QRCodeMultiReader.h>
#include <zxing/multi/ByQuadrantReader.h>
#include <zxing/multi/MultipleBarcodeReader.h>
#include <zxing/multi/GenericMultipleBarcodeReader.h>

#include <QRCodeGenerator/QrCode.hpp>

using namespace zxing;
using namespace zxing::multi;
using namespace zxing::qrcode;

static std::vector<Ref<Result>> decode(const Ref<BinaryBitmap> &image, const DecodeHints &hints)
{
    Ref<Reader> reader(new MultiFormatReader);
    return std::vector<Ref<Result>>(1, reader->decode(image, hints));
}

static int read_image(const Ref<LuminanceSource> &source, std::vector<Ref<Result>> &results, bool hybrid)
{
    int res = -1;

    try {
        Ref<Binarizer> binarizer;

        if (hybrid)
        {
            binarizer = new HybridBinarizer(source);
        }
        else
        {
            binarizer = new GlobalHistogramBinarizer(source);
        }

        DecodeHints hints(DecodeHints::DEFAULT_HINT);
        hints.setTryHarder(true);
        Ref<BinaryBitmap> binary(new BinaryBitmap(binarizer));
        results = decode(binary, hints);
        res = 0;
    } catch (const ReaderException&) {
        res = -2;
    } catch (const zxing::IllegalArgumentException&) {
        res = -3;
    } catch (const zxing::Exception&) {
        res = -4;
    } catch (const std::exception&) {
        res = -5;
    }

    return res;
}

bool QRCode::decode(const std::string &filename, std::string &data)
{
    if (filename.empty())
    {
        return false;
    }

    Ref<LuminanceSource> source;
    data.clear();

    try {
        source = ImageReaderSource::create(filename);
    } catch (const zxing::IllegalArgumentException&) {
        return false;
    }

    std::vector<Ref<Result>> results;

    int gresult = 1;
    int hresult = 1;

    // try hybrid mode first
    hresult = read_image(source, results, true);

    // if that failed try without hybrid mode
    if (hresult != 0)
    {
        gresult = read_image(source, results, false);
        if (gresult != 0)
        {
            return false;
        }
    }

    // read data
    for (auto&& res : results)
    {
//        for (auto&& val : res->getRawBytes()->values())
//        {
//            data += val;
//        }
        data += res->getText()->getText();
    }

    return true;
}

bool QRCode::encode(const std::string &input, std::string &out)
{
    // empty data can't be and should not be encoded
    if (input.empty())
    {
        return false;
    }

    const auto qr = qrcodegen::QrCode::encodeText(input.c_str(), qrcodegen::QrCode::Ecc::QUARTILE);
    out = qr.toSvgString(3);
    return true;
}
