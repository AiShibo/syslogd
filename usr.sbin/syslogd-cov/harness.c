#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define DATA_SIZE (1024 * 3)
#define MESSAGE_COUNT 5 /* Number of messages to generate */
#define SINGLE_MESSAGE_MAX_SIZE                                                \
  (sizeof(uint32_t) + 3 * (sizeof(uint64_t) + 255)) /* type + 3*(len+maxstr)   \
                                                     */
#define TOTAL_DATA_SIZE                                                        \
  (SINGLE_MESSAGE_MAX_SIZE * MESSAGE_COUNT) /* MESSAGE_COUNT messages */

/* Calculate input bytes consumed: length_bytes + (message_size -
 * header_overhead) */
#define CALC_INPUT_CONSUMED(length_bytes, msg_size, header_size)               \
  ((length_bytes) + ((msg_size) - (header_size)))

enum cmd_types {
  PRIV_OPEN_TTY = 0,          /* open terminal or console device */
  PRIV_OPEN_LOG = 1,          /* open logfile for appending */
  PRIV_OPEN_PIPE = 2,         /* fork & exec child that gets logs on stdin */
  PRIV_OPEN_UTMP = 3,         /* open utmp for reading only */
  PRIV_OPEN_CONFIG = 4,       /* open config file for reading only */
  PRIV_CONFIG_MODIFIED = 5,   /* check if config file has been modified */
  PRIV_GETADDRINFO = 6,       /* resolve host/service names */
  PRIV_GETNAMEINFO = 7,       /* resolve numeric address into hostname */
  PRIV_DONE_CONFIG_PARSE = 8, /* signal that initial config parse is done */
  PRIV_CMD_MAX = 9
};

// message PRIV_OPEN_TTY, PRIV_OPEN_LOG, PRIV_OPEN_PIPE, and PRIV_GETNAMEINFO
size_t build_single_str_message(uint8_t *buf, size_t buf_size,
                                uint8_t *mutated_out, uint32_t message_type) {
  uint32_t *type;
  uint64_t *str_len;
  size_t local_strlen;
  size_t out_sz = DATA_SIZE;
  size_t total_size = 0;


  /* check space for 32-bit type header */
  if (out_sz < sizeof(uint32_t))
    return 0;

  /* set the 32-bit message type */
  type = (uint32_t *)mutated_out;
  *type = message_type;
  mutated_out += sizeof(uint32_t);
  out_sz -= sizeof(uint32_t);
  total_size += sizeof(uint32_t);

  /* need at least one input byte for length */
  if (buf_size < 1)
    return total_size;

  /* check space for 64-bit length header */
  if (out_sz < sizeof(uint64_t))
    return total_size;

  /* set the 64-bit length header from buf[0] */
  str_len = (uint64_t *)mutated_out;
  *str_len = (uint64_t)buf[0];
  local_strlen = *str_len;
  mutated_out += sizeof(uint64_t);
  out_sz -= sizeof(uint64_t);
  total_size += sizeof(uint64_t);

  /* consume one byte of input */
  buf += 1;
  buf_size -= 1;

  /* check that payload fits in both input and output */
  if ((size_t)local_strlen > buf_size || local_strlen > out_sz) {
    return total_size;
  }

  /* copy the payload */
  memcpy(mutated_out, buf, local_strlen);
  total_size += local_strlen;


  return total_size;
}

// message PRIV_OPEN_UTMP, PRIV_OPEN_CONFIG, PRIV_CONFIG_MODIFIED, and
// PRIV_DONE_CONFIG_PARSE
size_t build_simple_message(uint8_t *mutated_out, uint32_t message_type) {
  uint32_t *type;

  /* set the 32-bit message type */
  type = (uint32_t *)mutated_out;
  *type = message_type;


  return sizeof(uint32_t);
}

// message PRIV_GETADDRINFO
size_t build_message_six(uint8_t *buf, size_t buf_size, uint8_t *mutated_out) {
  uint32_t *type;
  uint64_t *str_len;
  size_t local_strlen;
  size_t out_sz = DATA_SIZE;
  size_t total_size = 0;

  /* check space for 32-bit type header */
  if (out_sz < sizeof(uint32_t))
    return 0;

  /* set the 32-bit message type = PRIV_GETADDRINFO */
  type = (uint32_t *)mutated_out;
  *type = PRIV_GETADDRINFO;
  mutated_out += sizeof(uint32_t);
  out_sz -= sizeof(uint32_t);
  total_size += sizeof(uint32_t);

  /* need at least one input byte for first length */
  if (buf_size < 1)
    return total_size;

  /* check space for first 64-bit length header */
  if (out_sz < sizeof(uint64_t))
    return total_size;

  /* set the first 64-bit length header from buf[0], constrained to 3 or 4 */
  str_len = (uint64_t *)mutated_out;
  *str_len = (uint64_t)(buf[0] % 2 + 3); /* results in 3 or 4 */
  local_strlen = *str_len;
  mutated_out += sizeof(uint64_t);
  out_sz -= sizeof(uint64_t);
  total_size += sizeof(uint64_t);

  /* consume one byte of input */
  buf += 1;
  buf_size -= 1;

  /* check that first payload fits in both input and output */
  if ((size_t)local_strlen > buf_size || local_strlen > out_sz) {
    return total_size;
  }

  /* copy the first payload */
  memcpy(mutated_out, buf, local_strlen);
  mutated_out += local_strlen;
  out_sz -= local_strlen;
  buf += local_strlen;
  buf_size -= local_strlen;
  total_size += local_strlen;

  /* need at least one input byte for second length */
  if (buf_size < 1)
    return total_size;

  /* check space for second 64-bit length header */
  if (out_sz < sizeof(uint64_t))
    return total_size;

  /* set the second 64-bit length header from next byte, no max restrictions */
  str_len = (uint64_t *)mutated_out;
  *str_len = (uint64_t)buf[0];
  local_strlen = *str_len;
  mutated_out += sizeof(uint64_t);
  out_sz -= sizeof(uint64_t);
  total_size += sizeof(uint64_t);

  /* consume one byte of input */
  buf += 1;
  buf_size -= 1;

  /* check that second payload fits in both input and output */
  if ((size_t)local_strlen > buf_size || local_strlen > out_sz) {
    return total_size;
  }

  /* copy the second payload */
  memcpy(mutated_out, buf, local_strlen);
  mutated_out += local_strlen;
  out_sz -= local_strlen;
  buf += local_strlen;
  buf_size -= local_strlen;
  total_size += local_strlen;

  /* need at least one input byte for third length */
  if (buf_size < 1)
    return total_size;

  /* check space for third 64-bit length header */
  if (out_sz < sizeof(uint64_t))
    return total_size;

  /* set the third 64-bit length header from next byte, no restrictions */
  str_len = (uint64_t *)mutated_out;
  *str_len = (uint64_t)buf[0];
  local_strlen = *str_len;
  mutated_out += sizeof(uint64_t);
  out_sz -= sizeof(uint64_t);
  total_size += sizeof(uint64_t);

  /* consume one byte of input */
  buf += 1;
  buf_size -= 1;

  /* check that third payload fits in both input and output */
  if ((size_t)local_strlen > buf_size || local_strlen > out_sz) {
    return total_size;
  }

  /* copy the third payload */
  memcpy(mutated_out, buf, local_strlen);
  total_size += local_strlen;


  return total_size;
}

size_t custom_harness(uint8_t *buf, size_t buf_size, uint8_t **out_buf) {

  uint8_t *output_buffer = malloc(4000);
  uint8_t *current_out = output_buffer;
  size_t total_size = 0;
  size_t remaining_buf = buf_size;
  uint8_t *current_buf = buf;

  // Generate MESSAGE_COUNT messages
  for (int i = 0; i < MESSAGE_COUNT; i++) {
    if (remaining_buf < 1) {
      // Not enough input data, use the first byte for remaining messages
      current_buf = buf;
      remaining_buf = buf_size;
    }

    uint8_t message_type =
        current_buf[0] % PRIV_CMD_MAX; /* ensure message type is valid */
    uint8_t *msg_buf = current_buf + 1;
    size_t msg_buf_size = remaining_buf - 1;


    size_t msg_size = 0;
    size_t length_bytes = 0; /* Number of length bytes consumed from input */
    size_t header_size = 0;  /* Size of headers in output message */

    switch (message_type) {
    case PRIV_OPEN_TTY:
    case PRIV_OPEN_LOG:
    case PRIV_OPEN_PIPE:
    case PRIV_GETNAMEINFO:
      msg_size = build_single_str_message(msg_buf, msg_buf_size, current_out,
                                          message_type);
      length_bytes = 1; /* 1 length byte consumed */
      header_size = sizeof(uint32_t) + sizeof(uint64_t); /* type + length */
      break;

    case PRIV_OPEN_UTMP:
    case PRIV_OPEN_CONFIG:
    case PRIV_CONFIG_MODIFIED:
    case PRIV_DONE_CONFIG_PARSE:
      msg_size = build_simple_message(current_out, message_type);
      length_bytes = 0;               /* No input consumed */
      header_size = sizeof(uint32_t); /* type only */
      break;

    case PRIV_GETADDRINFO:
      msg_size = build_message_six(msg_buf, msg_buf_size, current_out);
      length_bytes = 3; /* 3 length bytes consumed */
      header_size =
          sizeof(uint32_t) + 3 * sizeof(uint64_t); /* type + 3 lengths */
      break;

    default:
      break;
    }

    current_out += msg_size;
    total_size += msg_size;

    // Move to next part of input buffer
    size_t consumed =
        1 + CALC_INPUT_CONSUMED(
                length_bytes, msg_size,
                header_size); /* 1 for message type + actual input consumed */
    if (consumed <= remaining_buf) {
      current_buf += consumed;
      remaining_buf -= consumed;
    } else {
      // Wrap around or restart from beginning
      current_buf = buf;
      remaining_buf = buf_size;
    }
  }

  *out_buf = output_buffer;
  return total_size;
}
