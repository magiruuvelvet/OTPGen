/*
 *  Copyright 2010-2011 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ImageReaderSource.h"
#include <zxing/common/IllegalArgumentException.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "lodepng.h"
#include "jpgd.h"
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION
#define NANOSVG_ALL_COLOR_KEYWORDS
#include "nanosvg.h"
#include "nanosvgrast.h"

using std::string;
using std::ostringstream;
using zxing::Ref;
using zxing::ArrayRef;
using zxing::LuminanceSource;

inline char ImageReaderSource::convertPixel(char const* pixel_) const {
  unsigned char const* pixel = (unsigned char const*)pixel_;
  if (comps == 1 || comps == 2) {
    // Gray or gray+alpha
    return pixel[0];
  } if (comps == 3 || comps == 4) {
    // Red, Green, Blue, (Alpha)
    // We assume 16 bit values here
    // 0x200 = 1<<9, half an lsb of the result to force rounding
    return (char)((306 * (int)pixel[0] + 601 * (int)pixel[1] +
        117 * (int)pixel[2] + 0x200) >> 10);
  } else {
    throw zxing::IllegalArgumentException("Unexpected image depth");
  }
}

ImageReaderSource::ImageReaderSource(ArrayRef<char> image_, int width, int height, int comps_)
    : Super(width, height), image(image_), comps(comps_) {}

Ref<LuminanceSource> ImageReaderSource::create(string const& filename) {
  string extension = filename.substr(filename.find_last_of(".") + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
  int width = 0, height = 0;
  int comps = 0;
  zxing::ArrayRef<char> image;

  // PNG (lodepng)
  if (extension == "png") {
    std::vector<unsigned char> out;

    { unsigned w, h;
      unsigned error = lodepng::decode(out, w, h, filename);
      if (error) {
        ostringstream msg;
        msg << "Error while loading '" << lodepng_error_text(error) << "'";
        throw zxing::IllegalArgumentException(msg.str().c_str());
      }
      width = w;
      height = h;
    }

    comps = 4;
    image = zxing::ArrayRef<char>(4 * width * height);
    memcpy(&image[0], &out[0], image->size());

  // JPEG (jpgd)
  } else if (extension == "jpg" ||
             extension == "jpe" ||
             extension == "jpeg") {
    char *buffer = reinterpret_cast<char*>(jpgd::decompress_jpeg_image_from_file(
        filename.c_str(), &width, &height, &comps, 4));
    image = zxing::ArrayRef<char>(buffer, 4 * width * height);
    free(buffer);

  // SVG (nanosvg)
  } else if (extension == "svg") {
      auto svgimage = nsvgParseFromFile(filename.c_str(), "px", 96);
      if (svgimage)
      {
          auto rasterizer = nsvgCreateRasterizer();
          auto buffer = static_cast<unsigned char*>(std::malloc(svgimage->width * svgimage->height * 4));
          nsvgRasterize(rasterizer, svgimage, 0, 0, 1, buffer, svgimage->width, svgimage->height, svgimage->width * 4);
          width = (int) svgimage->width;
          height = (int) svgimage->height;
          image = zxing::ArrayRef<char>(reinterpret_cast<char*>(buffer), width * height * 4);
          nsvgDelete(svgimage);
          nsvgDeleteRasterizer(rasterizer);
          std::free(buffer);
      }
  }

  if (!image) {
    ostringstream msg;
    msg << "Loading \"" << filename << "\" failed.";
    throw zxing::IllegalArgumentException(msg.str().c_str());
  }

  return Ref<LuminanceSource>(new ImageReaderSource(image, width, height, comps));
}

zxing::ArrayRef<char> ImageReaderSource::getRow(int y, zxing::ArrayRef<char> row) const {
  const char* pixelRow = &image[0] + y * getWidth() * 4;
  if (!row) {
    row = zxing::ArrayRef<char>(getWidth());
  }
  for (int x = 0; x < getWidth(); x++) {
    row[x] = convertPixel(pixelRow + (x * 4));
  }
  return row;
}

/** This is a more efficient implementation. */
zxing::ArrayRef<char> ImageReaderSource::getMatrix() const {
  const char* p = &image[0];
  zxing::ArrayRef<char> matrix(getWidth() * getHeight());
  char* m = &matrix[0];
  for (int y = 0; y < getHeight(); y++) {
    for (int x = 0; x < getWidth(); x++) {
      *m = convertPixel(p);
      m++;
      p += 4;
    }
  }
  return matrix;
}
