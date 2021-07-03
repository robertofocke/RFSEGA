#include <genesis.h>
#include "recursos.h"

int cantidadenemigos = 40; //cantidad de enemigos
int num_ene = 0;           //cantidad de enemigos en escena
int enemigosmuertos = 0;   //cantidad de enemigos muertos
Sprite *sprite_vampiro;    //sprite del personaje
Sprite *luna;              //sprite de la luna
int startgame = 0;         //bandera para iniciar juego
//char *s="Play";
struct Enemigo
{
    Sprite *sprite_disparo;
    int posx;
    int posy;
    bool mov;
};
int posx = 70;
int posy = 70;
u16 start;

int main()
{
    u16 ind = 1;
    start = getTime(500);
    //musica
    SND_setPCM_XGM(16, musica, sizeof(musica));
    SND_startPlayPCM_XGM(16, 1, SOUND_PCM_CH2);

    //cordenadas iniciales del enemigo
    struct Enemigo unEnemigo[cantidadenemigos];

    VDP_setPlanSize(32, 64);
    SYS_disableInts();
    u16 anim = 0;
    s16 offset_H_PlanoB = -2;

    VDP_setPalette(PAL0, estrellas.palette->data);
    ind = TILE_USERINDEX;
    VDP_drawImageEx(BG_B, &estrellas, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, CPU);
    ind += estrellas.tileset->numTile;

    SPR_init(0);
    VDP_setPalette(PAL1, moon.palette->data);
    luna = SPR_addSprite(&moon, 0, 0, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    SYS_enableInts();
    VDP_setPalette(PAL3, disparo.palette->data);

    VDP_setPalette(PAL2, vampiro.palette->data);
    sprite_vampiro = SPR_addSprite(&vampiro, posx, posy, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

    SPR_setAnim(sprite_vampiro, anim);
    SPR_update();

    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    while (TRUE)
    {
        SYS_doVBlankProcess();

        if (startgame == 1)
        {

            SPR_update();
            SPR_setAnim(sprite_vampiro, anim);
            readcontrollers();
            generardisparo(unEnemigo);
            updateposenemigo(unEnemigo);
            VDP_setHorizontalScroll(BG_B, offset_H_PlanoB);
            offset_H_PlanoB = offset_H_PlanoB - 1;
        }
        else if (startgame == 2)
        {
            for (int i = 0; i < num_ene; i++)
            {
                SPR_releaseSprite(unEnemigo[i].sprite_disparo);
            }
            SPR_update();
            enemigosmuertos=0;
            num_ene = 0;
            char *s = "Game Over";
            int value = JOY_readJoypad(JOY_1);
            VDP_drawText(s, 20 - strlen(s) / 2, 15);
           
            if (value & BUTTON_RIGHT)
            {

                VDP_clearTextLine(15);
                startgame = 0;
            }
            //menu("Play",unEnemigo,1);
        }
        else if (startgame == 3)
        {
            enemigosmuertos=0;
            num_ene = 0;
            char *s = "You Win";
            int value = JOY_readJoypad(JOY_1);
            VDP_drawText(s, 20 - strlen(s) / 2, 15);
            if (value & BUTTON_LEFT)
            {
                VDP_clearTextLine(15);
                startgame = 0;
            }
        }
        else if (startgame == 0)
        {
            char *s = "Play Game";

            int value = JOY_readJoypad(JOY_1);
            VDP_drawText(s, 20 - strlen(s) / 2, 15);
            if (value & BUTTON_START)
            {
                for (int z = 0; z < cantidadenemigos; z++)
                {
                    unEnemigo[z].posy = 100;
                    unEnemigo[z].posx = 300;
                    unEnemigo[z].mov = FALSE;
                }
                VDP_clearTextLine(15);
                startgame = 1;
            }
        }
    }
    return 0;
}

void generardisparo(struct Enemigo unEnemigo[cantidadenemigos])
{
    u16 dif = 0;
    int aux_pos = (random() % (224 - 1 + 1)) + 1; //numero aleatorio entre 1 y 224
    if (num_ene < cantidadenemigos)
    {
        dif = getTime(500) - start;
        if (dif > 300)
        {
            unEnemigo[num_ene].sprite_disparo = SPR_addSprite(&disparo, unEnemigo[num_ene].posx, unEnemigo[num_ene].posy, TILE_ATTR(PAL3, TRUE, FALSE, TRUE));
            unEnemigo[num_ene].mov = TRUE;
            num_ene = num_ene + 1;
            unEnemigo[num_ene].posy = aux_pos;
            start = getTime(500);
        }
    }
}

void updateposenemigo(struct Enemigo unEnemigo[cantidadenemigos])
{
    if (enemigosmuertos == cantidadenemigos)
    {
        startgame = 3;
    }
    else
    {
        for (int z = 0; z < cantidadenemigos; z++)
        {
            if (unEnemigo[z].mov)
            {
                if (unEnemigo[z].posx > posx - 10 && unEnemigo[z].posx < posx + 10 && unEnemigo[z].posy > posy - 25 && unEnemigo[z].posy < posy + 40)
                {
                    SPR_releaseSprite(unEnemigo[z].sprite_disparo);
                    enemigosmuertos++;
                    startgame = 2;
                    //menu("Game Over",unEnemigo,0);
                }
                else if (unEnemigo[z].posx <= 0)
                {
                    unEnemigo[z].mov = FALSE;
                    SPR_releaseSprite(unEnemigo[z].sprite_disparo);
                    enemigosmuertos++;
                }
                else
                {
                    SPR_setPosition(unEnemigo[z].sprite_disparo, unEnemigo[z].posx--, unEnemigo[z].posy);
                }
            }
        }
    }
}

void readcontrollers()
{
    //Se lee el estado del joistick en el puerto 1
    int value = JOY_readJoypad(JOY_1);
    if (value & BUTTON_DOWN && (posy) <= 150)
    {
        SPR_setPosition(sprite_vampiro, posx, posy++);
    }

    if (value & BUTTON_UP && (posy) >= 1)
    {
        SPR_setPosition(sprite_vampiro, posx, posy--);
    }
}

//void menu(char *s, struct Enemigo unEnemigo[cantidadenemigos], int state);
void updateposenemigo(struct Enemigo unEnemigo[cantidadenemigos]);
void readcontrollers();
void generardisparo(struct Enemigo unEnemigo[cantidadenemigos]);
