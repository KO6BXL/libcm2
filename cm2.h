#ifndef LIBCM2_LIBRARY_H
#define LIBCM2_LIBRARY_H
#include <stdbool.h>

typedef struct color {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} color;

typedef struct offset {
    float x;
    float y;
    float z;
} offset;

typedef struct base {
    char name[12];
    unsigned char id;
    int properties[6];
    color color;
    offset offset;
    bool state;
} base;

typedef struct connection {
    unsigned long from;
    unsigned long to;
} connection;

typedef struct ll_connection {
    connection val;
    struct ll_connection *next;
} ll_connection;

typedef struct led_params {
    color color;
    unsigned char opacity_on;
    unsigned char opacity_off;
    bool analog;
    bool do_I_exist;
} led_params;

typedef struct collection {
    base *blocks;
    unsigned long block_len;
    connection *connections;
    unsigned long connection_len;
    offset *position;
} collection;

/**
 * @brief Initializes a new collection
 * @return New collection
 */
 collection *new_collection();

/**
 * @brief Prints debug statements about a block
 * @param Block to debug
 */
void block_debug(base);
/**
 * @brief Offset a block
 * @param Index of block
 * @param Collection that has the block
 * @param Offset values
 */
void mod_offset(unsigned long, collection*, offset);

/**
 * @brief Add new block to collection
 * @param Block to add, typically from block_<name>()
 * @param Existing collection, made with new_collection()
 * @return Index in block array
 */
unsigned long append( base,  collection*);

/**
 * @brief Connect two blocks in a collection together
 * @param Block to connect from
 * @param Block to connect to
 * @param Collection from new_collection()
 */
void connect(unsigned long, unsigned long,  collection*);

/**
 * @brief Merges two collections into one. NOTE: both collections are destroyed after merge!
 * @param Collection one; holds the result of the merge
 * @param Collection two; appended to collection one
 */
collection *merge( collection*,  collection*);

/**
 * @brief Frees up memory in collection
 * @param Collection to clean
 */
void clean_cm2( collection*);

/**
 * @brief Compiles a collection into a usable save string
 * @param Collection to compile
 * @return Memory address of created string
 */
char *compile(collection*);

/**
 * @brief Generates a nor gate
 * @return New nor gate
 */
 base block_nor();

/**
 * Generates an and gate
 * @return New and gate
 */
 base block_and();

/**
 * @brief Generates an or gate
 * @return New or gate
 */
 base block_or();

/**
 * @brief Generates a xor gate
 * @return New xor gate
 */
 base block_xor();

/**
 * @brief Generates a button
 * @return New button
 */
 base block_button();

/**
 * @brief Generates a T FlipFlop
 * @return New T FlipFlop
 */
 base block_flipflop();

/**
 * @brief Generates an LED with given params
 * @param Options for LED; if null defaults are used
 * @return New LED
 */
 base block_led( led_params);

/**
 * @brief Generates a sound block with given params
 * @param Frequency of the sound
 * @param Instrument to use.
 * @return New sound block
 */
 base block_sound(unsigned int, unsigned char);

/**
 * @brief Generates a conductor
 * @return New condunctor
 */
 base block_conductor();

/**
 * @brief Generates a custom block
 * @return New custom block
 */
 base block_custom();

/**
 * @brief Generates a nand gate
 * @return New nand gate
 */
 base block_nand();

/**
 * @brief Generates a xnor gate
 * @return New xnor gate
 */
 base block_xnor();

/**
 * @brief Generates a random block
 * @return New random block
 */
 base block_random();

/**
 * @brief Generates a text block
 * @param ASCII character dispalyed on the block
 * @return New text block
 */
 base block_text(unsigned char);

/**
 * @brief Generates a tile block
 * @param Color of the block; CANNOT BE NULL!
 * @param Material for the block; also CANNOT BE NULL!
 * @return New tile block
 */
 base block_tile( color, unsigned char);

/**
 * @brief Generates a delay block
 * @param Amount of ticks to delay for
 * @return New delay block
 */
 base block_delay(unsigned int);

/**
 * @brief Generates an antenna block
 * @param Channel for the antenna to operate on
 * @return New antenna block
 */
 base block_antenna(unsigned int);

/**
 * @brief Generates a conductorV2 block
 * @return New conductorV2 block
 */
 base block_conductorV2();
#endif //LIBCM2_LIBRARY_H