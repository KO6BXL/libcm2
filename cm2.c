#include "cm2.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

collection *new_collection() {
    base bases = {0};
    connection connections = {0};
    offset offset = {0};

    base *base_p = malloc(sizeof(bases));
    *base_p = bases;

    connection *connection_p = malloc(sizeof(connections));
    *connection_p = connections;

    struct offset *offset_p = malloc(sizeof(offset));
    *offset_p = offset;

    collection col = {base_p, 0, connection_p, 0, offset_p};

    collection *col_p = malloc(sizeof(col));

    *col_p = col;

    return col_p;
}

void mod_offset(unsigned long index, collection *col, offset offset) {
    base block = col->blocks[index];
    block.offset = offset;
    col->blocks[index] = block;
}

unsigned long append(base block, collection *col) {
    unsigned long block_amount = sizeof(base) * (col->block_len + 1);
    base *new_blocks = malloc(block_amount);
    if (new_blocks == 0) {
        fprintf(stderr,"Failed to allocate memory!");
        return 0;
    }
    memcpy(new_blocks, col->blocks, sizeof(base) * col->block_len);
    free(col->blocks);
    col->blocks = new_blocks;
    col->blocks[col->block_len++] = block;
    return col->block_len;
}

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

collection *merge( collection *one,  collection *two) {
    unsigned long total_size = sizeof(one) + sizeof(two);
    collection *new_col = malloc(total_size);
    new_col = memcpy(new_col, one, sizeof(one));
    new_col = memcpy(new_col+sizeof(one), two, sizeof(two));
    clean_cm2(one);
    clean_cm2(two);
    return new_col;
}

 base clean_block( base block) {
     base new_block = block;
    for (int i = 0; i<5; i++) {
        new_block.properties[i] = 0;
    }
    new_block.offset = ( offset) {0,0,0};
    new_block.state = false;
    return new_block;
}

void clean_cm2( collection *col) {
    free(col->blocks);
    free(col->connections);
    free(col);
}

void build_block_str(base block, char *buf) {
    char idbuf[2] = {0};
    sprintf(idbuf, "%u,", block.id);
    buf = stpcpy(buf, idbuf);

    if (block.state) {
        buf = stpcpy(buf, "1,");
    } else {
        buf = stpcpy(buf, "0,");
    }

    char offset_buf[15];
    sprintf(offset_buf, "%g,", block.offset.x);
    buf = stpcpy(buf, offset_buf);

    sprintf(offset_buf, "%g,", block.offset.y);
    buf = stpcpy(buf, offset_buf);

    sprintf(offset_buf, "%g,", block.offset.y);
    buf = stpcpy(buf, offset_buf);


    char prop_buf[5] = {0};
    for (int i = 0; i < 6; i++) {
        if (block.properties[i] == 0) continue;
        printf(prop_buf, "%d", block.properties[i]);
        buf = stpcpy(buf, prop_buf);

        if (i != 5) {
            buf = stpcpy(buf, "+");
        }
    }
    buf = stpcpy(buf, ";");
}

char* compile(collection *col) {
    char *collection_buf = 0;
    unsigned long block_count = col->block_len;
    block_count++;
    char *block_buf = malloc(50);
    for (unsigned long i = 0; i < block_count; i++) {
        build_block_str(col->blocks[i], block_buf);
        char *new_col_buf = 0;
        if (collection_buf == 0) {
            new_col_buf = malloc(strlen(block_buf));
        } else {
            new_col_buf = malloc(strlen(collection_buf)+strlen(block_buf));
        }
        if (new_col_buf == 0) {
            fprintf(stderr, "Failed to allocate collection buffer!");
            free(block_buf);
            free(collection_buf);
            return 0;
        }
        char *p = new_col_buf;
        if (collection_buf != 0) {
            p = stpcpy(p, collection_buf);
        }
        stpcpy(p, block_buf);
        collection_buf = new_col_buf;
    }

    free(block_buf);
    return collection_buf;
}


//Now we start with the blocks

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
    bool use_default = false;
    if (params.do_I_exist == false) {
        params = ( led_params) {( color) {175, 175, 175}, 100, 25, false, true};
        use_default = true;
    }

     base led;
    strcpy(led.name, "LED");
    led.id = 6;
    led.color = params.color;
    int analog_on = 0;
    if (params.analog == true) {
        analog_on = 1;
    }
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