#include <vector>
#include <exception>
#include <string>
#include <sstream>
#include <cinttypes>

extern "C" {
#include "EBS.h"
}


namespace EBS {
    enum class ErrorType {
        OOM = EBS_ErrorOOM,
        InvalidMessage = EBS_ErrorInvalidMessage,
        Overflow = EBS_ErrorOverflow,
        BadSquareSize = EBS_ErrorBadSquareSize,
        InvalidImage = EBS_ErrorInvalidImage
    };

    class Error : public std::exception {
    private:
        const ErrorType error;
        std::string message{};

    public:
        explicit Error(ErrorType errorType) : error{errorType} {
            std::stringstream ss;
            ss << "EBS error: " << static_cast<int>(this->error);
            ss >> this->message;
        }

        const char *what() const noexcept override { return this->message.c_str(); }

        ErrorType errorType() const noexcept { return this->error; }
    };

    class Image {
    public:
        const uint64_t width, height, channel;
        const std::shared_ptr<std::vector<uint8_t>> pixels;

        Image(uint64_t width, uint64_t height, uint64_t channel, const std::shared_ptr<std::vector<uint8_t>> &pixels) :
            width{width}, height{height}, channel{channel}, pixels{pixels} {}

        EBS_Image toEBS() const {
            EBS_Image ebsImage{width, height, channel, const_cast<uint8_t *>(pixels->data())};
            return ebsImage;
        }
    };

    typedef std::vector<std::shared_ptr<Image>> ImageList;
    typedef std::vector<uint8_t> Data;

    class Message {
    private:
        const uint64_t squareSize;
    public:
        explicit Message(uint64_t squareSize) : squareSize{squareSize} {}

        void embed(ImageList &imageList, const Data &data) const {
            auto images = new EBS_Image[imageList.size()];
            for (uint64_t i = 0; i < imageList.size(); ++i) {
                images[i] = imageList[i]->toEBS();
            }
            EBS_ImageList ebsImageList{imageList.size(), images};
            EBS_Message ebsMessage{data.size(), const_cast<uint8_t *>(data.data())};
            int errorCode;
            EBS_MessageEmbed(&ebsImageList, &ebsMessage, this->squareSize, &errorCode);
            if (errorCode != EBS_OK) {
                delete[] images;
                throw Error{static_cast<ErrorType>(errorCode)};
            }
            delete[] images;
        }

        Data extract(const ImageList &imageList) const {
            auto images = new EBS_Image[imageList.size()];
            for (uint64_t i = 0; i < imageList.size(); ++i) {
                images[i] = imageList[i]->toEBS();
            }
            EBS_ImageList ebsImageList{imageList.size(), images};
            int errorCode;
            EBS_Message ebsMessage = EBS_MessageExtract(&ebsImageList, this->squareSize, &errorCode);
            if (errorCode != EBS_OK) {
                delete[] images;
                throw Error{static_cast<ErrorType>(errorCode)};
            }
            delete[] images;
            Data data{ebsMessage.data, ebsMessage.data + ebsMessage.size};
            return data;
        }
    };
}
