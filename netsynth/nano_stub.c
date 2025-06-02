#include <pb_encode.h>
#include <pb_decode.h>

#include "nano_connector.pb.h"

size_t generateStream(uint8_t* buffer, size_t size)
{
    connector_Component component = {};

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);
    pb_encode(&stream, connector_Component_fields, &component);

    return stream.bytes_written;
}
