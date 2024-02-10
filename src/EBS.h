#pragma once

#include <inttypes.h>

/**
 * OK Code.
 * It indicates that no error occurred.
 */
static const int EBS_OK = 0;

/**
 * Out of Memory Error.
 * Could occur when embedding or extracting messages.
 * It indicates that your computer doesn't have enough memory to do the operations.
 */
static const int EBS_ErrorOOM = 1;

/**
 * Invalid Message Error.
 * Could occur when extracting messages.
 * It indicates that the length of data extracted from the images is larger than they could possibly hold.
 */
static const int EBS_ErrorInvalidMessage = 2;

/**
 * Overflow Error.
 * Could occur when embedding messages.
 * It indicates that the length of the message is large than what the images could possibly hold.
 */
static const int EBS_ErrorOverflow = 3;

/**
 * Bad Square Size.
 * Could occur when embedding or extracting messages.
 * It indicates that your square size doesn't meet the following requirements:
 * \code{.c}
 * squareSize != 0 && squareSize % 4 == 0 && squareSize < 255
 * \endcode
 */
static const int EBS_ErrorBadSquareSize = 4;

/**
 * Invalid Image.
 * Could occur when embedding or extracting messages.
 * It indicates that one or more of the images passed do not meet the following requirements:
 * \code{.c}
 * image->width != 0 && image->height != 0 && image->channel != 0 && image->pixels != NULL
 * \endcode
 */
static const int EBS_ErrorInvalidImage = 5;

typedef struct EBS_Image {
    uint64_t width;
    uint64_t height;
    uint64_t channel;
    uint8_t *pixels;
} EBS_Image;

typedef struct EBS_ImageList {
    uint64_t size;
    EBS_Image *images;
} EBS_ImageList;

typedef struct EBS_Message {
    uint64_t size;
    uint8_t *data;
} EBS_Message;

/**
 * @brief Embed a \b Message into an \b ImageList.
 * @param imageList A list of images to embed into. The memory should be handled by the caller.
 * @param message The message to embed. The memory should be handled by the caller.
 * @param squareSize The size of squares the image is split into to calculate local entropy. No larger than 256.
 * @param errorCode The error code if there's any. If there's no error, EBS_OK/0 is set.
 *
 * Note that the same squareSize is needed when the message is extracted, otherwise you might get wrong data.
 */
void EBS_MessageEmbed(EBS_ImageList *imageList, const EBS_Message *message, uint64_t squareSize, int *errorCode);

/**
 * @brief Extract a \b Message from an \b ImageList.
 * @param imageList A list of images to extract from. The memory should be handled by the caller.
 * @param squareSize The size of squares the image is split into to calculate local entropy. No larger than 256.
 * @param errorCode The error code if there's any. If there's no error, EBS_OK/0 is set.
 * @return The message extracted. The memory needs to be freed by the caller by calling
 * \code{.c}
 * free(message.data);
 * \endcode
 *
 * Note that the squareSize has to be the same as when the message was embedded, otherwise you might get wrong data.
 */
EBS_Message EBS_MessageExtract(EBS_ImageList *imageList, uint64_t squareSize, int *errorCode);

/**
 * @brief Free a \b Message returned by \b EBS_MessageExtract.
 * It's equal to
 * \code{.c}
 * free(message->data);
 * message->data = NULL;
 * \endcode
 * @param message The message to be freed. message.data will be set to NULL
 */
void EBS_MessageFree(EBS_Message *message);
