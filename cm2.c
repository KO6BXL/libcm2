#include "cm2.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Creates a collection
collection *new_collection() {
    //Declare empty values for the collection struct
    base bases = {0};
    connection connections = {0};
    offset offset = {0};

    //Allocate for blocks
    base *base_p = malloc(sizeof(bases));
    *base_p = bases;

    //Allocate for connections
    connection *connection_p = malloc(sizeof(connections));
    *connection_p = connections;

    //Allocate for offset
    //TODO: Get rid of offset
    struct offset *offset_p = malloc(sizeof(offset));
    *offset_p = offset;

    //Create final struct
    collection col = {base_p, 0, connection_p, 0, offset_p};
    collection *col_p = malloc(sizeof(col));
    *col_p = col;
    //Return pointer to collection
    return col_p;
}

//Offset block
void mod_offset(unsigned long index, collection *col, offset offset) {
    //Get block from collection
    base block = col->blocks[index];
    //Set offset
    block.offset = offset;
    //Write back to collection
    col->blocks[index] = block;
}

//Add a block to collection
unsigned long append(base block, collection *col) {
    //Get bytesize of the new block array
    unsigned long block_amount = sizeof(base) * (col->block_len + 1);
    //Allocate new space
    base *new_blocks = malloc(block_amount);
    if (new_blocks == 0) {
        fprintf(stderr,"Failed to allocate memory!");
        return 0;
    }
    //copy old data to the new space
    memcpy(new_blocks, col->blocks, sizeof(base) * col->block_len);
    //Replace old pointer with new
    free(col->blocks);
    col->blocks = new_blocks;
    //Append block and return its index (don't ask why the "++" and "- 1" are there, it works)
    col->blocks[col->block_len++] = block;
    return col->block_len - 1;
}

//connect block from to block to in collection
//THIS FUNCTION IS NOT TESTED!
//TODO: Test this function and fix any errors
void connect(unsigned long from, unsigned long to, collection *col) {
    connection conn = (connection) {from, to};
    unsigned long conn_amount = sizeof(connection) * (col->connection_len + 1);
    connection *new_conn = malloc(conn_amount);
    if (new_conn == 0) {
        fprintf(stderr, "Failed to allocate memory!");
        return;
    }
    new_conn = memcpy(new_conn, col->connections, col->connection_len);
    free(col->connections);
    col->connections = new_conn;
    col->connections[col->connection_len++] = conn;
}

//merge two collections together
//THIS FUNCTION IS NOT TESTED!
//TODO: Test this function and fix any errors
collection *merge( collection *one,  collection *two) {
    unsigned long total_size = sizeof(one) + sizeof(two);
    collection *new_col = malloc(total_size);
    new_col = memcpy(new_col, one, sizeof(one));
    new_col = memcpy(new_col+sizeof(one), two, sizeof(two));
    clean_cm2(one);
    clean_cm2(two);
    return new_col;
}

//helper function for block constructors
//NOTE: This function wipes the offset, properties, and state; so don't use it after setting these attributes
 base clean_block(base block) {
    //new block to eventually return
    base new_block = block;
    for (int i = 0; i<5; i++) {
        new_block.properties[i] = 0;
    }
    new_block.offset = ( offset) {0,0,0};
    new_block.state = false;
    return new_block;
}

//Cleans up memory in project
//This function has not been tested
//TODO: Test this function and fix any bugs
void clean_cm2( collection *col) {
    free(col->blocks);
    free(col->connections);
    free(col);
}

//Helper function for compilation
//Builds a string in buf based on data in block
void build_block_str(base block, char *buf) {
    //create a pointer to the end of the buffer
    char *buf_end = buf;
    //buffer to store block id
    char idbuf[2] = {0};
    sprintf(idbuf, "%u,", block.id);
    //Write to buffer
    buf_end = stpcpy(buf, idbuf);

    //if the state is true (useful for setting state in flipflops)
    if (block.state) {
        buf_end = stpcpy(buf_end, "1,");
    } else {
        buf_end = stpcpy(buf_end, "0,");
    }

    //Create and write offsets
    //TODO: Figure out a way to stop it from writing in scientific notation whenever the data gets too small
    char offset_buf[15];
    sprintf(offset_buf, "%g,", block.offset.x);
    buf_end = stpcpy(buf_end, offset_buf);

    sprintf(offset_buf, "%g,", block.offset.y);
    buf_end = stpcpy(buf_end, offset_buf);

    sprintf(offset_buf, "%g,", block.offset.z);
    buf_end = stpcpy(buf_end, offset_buf);

    //Create and write properties
    char prop_buf[5] = {0};
    for (int i = 0; i < 6; i++) {
        //If there is no property, don't waste time on it
        //TODO: This is a really bad way to do it, if someone wanted a color value of 0, it's not put in the final string. We need a way to track properties
        if (block.properties[i] == 0) continue;
        printf(prop_buf, "%d", block.properties[i]);
        buf_end = stpcpy(buf_end, prop_buf);

        //stop trailing "+"
        if (i != 5) {
            buf_end = stpcpy(buf_end, "+");
        }
    }
}

//Compiles collection into a string
//TODO: Finish function by adding connection functionality
char* compile(collection *col) {
    //create the main buffer to store output
    char *collection_buf = 0;
    unsigned long block_count = col->block_len;
    //Genuinely I have no idea what this is doing here, perhaps to stop garbage from being spat out by the for loop?
    block_count++;
    //Create buffer for the helper function to write into
    char *block_buf = malloc(50);
    for (unsigned long i = 0; i < block_count-1; i++) {
        //build block
        build_block_str(col->blocks[i], block_buf);
        //set new buffer for reallocation
        char *new_col_buf = 0;
        //check if we need to include the size of the main buffer
        if (collection_buf == 0) {
            new_col_buf = malloc(strlen(block_buf)+1);
        } else {
            new_col_buf = malloc(strlen(collection_buf)+strlen(block_buf));
        }
        //allocation check
        if (new_col_buf == 0) {
            fprintf(stderr, "Failed to allocate collection buffer!");
            free(block_buf);
            free(collection_buf);
            return 0;
        }
        //pointer to the end of the new buffer
        char *p = new_col_buf;
        if (collection_buf != 0) {
            //append old data
            p = stpcpy(p, collection_buf);
        }
        //append block string
        p = stpcpy(p, block_buf);
        //if it is the end of the block string, we don't want to write a semicolon
        if (i < block_count-2) {
            stpcpy(p, ";");
        }
        //point the main buffer to the new one
        //TODO: I am fairly positive this causes a memory leak, as the main buffer is never freed.
        collection_buf = new_col_buf;
    }
    //free memory (no way)
    free(block_buf);

    //TODO: Add connection functionality here

    //return string
    return collection_buf;
}

//helper function to block_debug()
void color_debug(color color) {
    printf("R: %d G: %d B: %d\n", color.R, color.G, color.B);
}

//helper function to block_debug()
void offset_debug(offset offset) {
    printf("X: %g Y: %g Z: %g\n", offset.x, offset.y, offset.z);
}
//Print out data about the block, use this to check blocks in the likely event this code breaks
void block_debug(base block) {
    printf("Start of block debug\n");
    printf("Block ID: %d\nBlock Name: %s\nBlock state: %d\n", block.id, block.name, block.state);
    color_debug(block.color);
    offset_debug(block.offset);
    for (int i = 0; i < 5; i++) {
        if (block.properties[i] == 0) continue;
        printf("Prop #%d: %d\n", i, block.properties[i]);
    }
    printf("End of block debug\n");
}

//Now we start with the blocks
//I am not writing comments for each of these, however led will have comments
 base block_nor() {
     base nor;
     color nor_color = {255, 9, 0};
    strcpy(nor.name, "NOR");
    nor.id = 0;
    nor.color = nor_color;

    nor = clean_block(nor);
    return nor;
}

 base block_and() {
     base and;
     color and_color = {0, 121, 255};
    strcpy(and.name, "AND");
    and.id = 1;
    and.color = and_color;

    and = clean_block(and);
    return and;
}

 base block_or() {
     base or;
     color or_color = {0, 241, 29};
    strcpy(or.name, "OR");
    or.id = 2;
    or.color = or_color;

    or = clean_block(or);
    return or;
}

 base block_xor() {
     base xor;
     color xor_color = {168, 0, 255};
    strcpy(xor.name, "XOR");
    xor.id = 3;
    xor.color = xor_color;

    xor = clean_block(xor);
    return xor;
}

 base block_button() {
     base button;
     color button_color = {255, 127, 0};
    strcpy(button.name, "BUTTON");
    button.id = 4;
    button.color = button_color;

    button = clean_block(button);
    return button;
}

 base block_flipflop() {
     base flipflop;
     color flipflop_color = {30, 30, 30};
    strcpy(flipflop.name, "FLIPFLOP");
    flipflop.id = 5;
    flipflop.color = flipflop_color;

    flipflop = clean_block(flipflop);
    return flipflop;
}

 base block_led( led_params params) {
    //are we using defaults?
    bool use_default = false;
    if (params.do_I_exist == false) {
        //defaults in case user provided NULL
        params = ( led_params) {( color) {175, 175, 175}, 100, 25, false, true};
        use_default = true;
    }

    base led;
    strcpy(led.name, "LED");
    led.id = 6;
    led.color = params.color;
    //used later for properties
    int analog_on = 0;
    if (params.analog == true) {
        analog_on = 1;
    }
    //clean before editing
    led = clean_block(led);
    if (use_default == false) {
        led.properties[0] = (int) params.color.R;
        led.properties[1] = (int) params.color.G;
        led.properties[2] = (int) params.color.B;
        led.properties[3] = (int) params.opacity_on;
        led.properties[4] = (int) params.opacity_off;
        led.properties[5] = analog_on;
    }
    return led;
}

 base block_sound(unsigned int frequency, unsigned char instrument) {
    base sound;
    color sound_color = {175, 131, 76};
    strcpy(sound.name, "SOUND");
    sound.id = 7;
    sound.color = sound_color;

    sound = clean_block(sound);
    sound.properties[0] = (int) frequency;
    sound.properties[1] = (int) instrument;

    return sound;
}

 base block_conductor() {
     base conductor;
     color conductor_color = {73, 185, 255};
    strcpy(conductor.name, "CONDUCTOR");
    conductor.id = 8;
    conductor.color = conductor_color;

    conductor = clean_block(conductor);
    return conductor;
}

 base block_custom() {
     base custom;
     color custom_color = {0, 42, 89};
    strcpy(custom.name, "CUSTOM");
    custom.id = 9;
    custom.color = custom_color;

    custom = clean_block(custom);
    return custom;
}

 base block_nand() {
     base nand;
     color nand_color = {0, 42, 89};
    strcpy(nand.name, "NAND");
    nand.id = 10;
    nand.color = nand_color;

    nand = clean_block(nand);
    return nand;
}

 base block_xnor() {
     base xnor;
     color xnor_color = {213, 0, 103};
    strcpy(xnor.name, "XNOR");
    xnor.id = 11;
    xnor.color = xnor_color;

    xnor = clean_block(xnor);
    return xnor;
}

 base block_random() {
     base random;
     color random_color = {94, 54, 35};
    strcpy(random.name, "RANDOM");
    random.id = 12;
    random.color = random_color;

    random = clean_block(random);
    return random;
}

 base block_text(unsigned char character) {
     base text;
     color text_color = {25, 71, 84};
    strcpy(text.name, "TEXT");
    text.id = 13;
    text.color = text_color;

    text = clean_block(text);
    text.properties[0] = (int) character;
    return text;
}

 base block_tile( color color, unsigned char material) {
     base tile;
    strcpy(tile.name, "TILE");
    tile.id = 14;
    tile.color = color;

    tile = clean_block(tile);
    tile.properties[0] = (int) color.R;
    tile.properties[1] = (int) color.G;
    tile.properties[2] = (int) color.B;
    tile.properties[3] = (int) material;
    return tile;
}

 base block_node() {
     base node;
     color node_color = {165, 177, 200};
    strcpy(node.name, "NODE");
    node.id = 15;
    node.color = node_color;

    node = clean_block(node);
    return node;
}

 base block_delay(unsigned int ticks) {
     base delay;
     color delay_color = {98, 24, 148};
    strcpy(delay.name, "DELAY");
    delay.id = 16;
    delay.color = delay_color;

    delay = clean_block(delay);
    delay.properties[0] = (int) ticks;
    return delay;
}

 base block_antenna(unsigned int channel) {
     base antenna;
     color antenna_color = {235, 233, 183};
    strcpy(antenna.name, "ANTENNA");
    antenna.id = 17;
    antenna.color = antenna_color;

    antenna = clean_block(antenna);
    antenna.properties[0] = (int) channel;
    return antenna;
}

 base block_conductorV2() {
     base conductorV2;
     color conductorV2_color = {52, 132, 182};
    strcpy(conductorV2.name, "CONDUCTORV2");
    conductorV2.id = 18;
    conductorV2.color = conductorV2_color;

    conductorV2 = clean_block(conductorV2);
    return conductorV2;
}