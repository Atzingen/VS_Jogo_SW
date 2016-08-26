/*
	Gustavo Voltani von Atzingen agosto 2016

	Jogo para aprendizado do allegro e c++

	comentarui de teste 
*/

// Coisas do c++
#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <time.h>
#include "string"

// Includes do alegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std; // bizarro

// Minhas funções e macros
void escreve_texto_opcoes(vector<string> opcoes, int escolhido);
int inicializar_allegro(void);
void the_end_allegro(void);
int fila_allegro(int tipo);
void cutscene(int tipo);
void inicio_setup(void);
void jogo(void);
void tela_configuracao(void);
void tela_login(void);
void jogo_pong(void);

// Variáveis/structs públicas
const int L = 1200;
const int A = 800;
float fps = 60;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_EVENT evento;
ALLEGRO_TIMEOUT timeout;
ALLEGRO_BITMAP *imagem = NULL;
ALLEGRO_BITMAP *bola = NULL;
ALLEGRO_BITMAP *barra = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;		// TODO
ALLEGRO_SAMPLE *som_setup;
ALLEGRO_SAMPLE *som_fim;
ALLEGRO_SAMPLE *som_abertura;
ALLEGRO_SAMPLE_ID id;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_TIMER *timer = NULL;

// TODO - definir as cores -> ALLEGRO_COLOR amarelo = al_map_rgb(255,255,0); ???
int tem_eventos;
float tempo_abertura = 10.0;
float tempo_final = 10.0;

int main(int argc, char **argv)
{
	srand(time(NULL));
	if (inicializar_allegro())
	{
		cutscene(1);
		inicio_setup();
		cutscene(2);
	}
	the_end_allegro();
	return 0;
}

void tela_login()
{

}

void jogo()
{

}

void jogo_pong()
{
	float raio_bola = al_get_bitmap_width(bola);
	float largura_barra = al_get_bitmap_width(barra);
	float altura_barra = al_get_bitmap_height(barra);
	bool desenhe_tela = false;
	int i = 0;
	float vx = 3.0*rand() / RAND_MAX + 2;
	float vy = 3.0*rand() / RAND_MAX + 2;
	fprintf(stderr, "vx = %f , vy= %f \n", vx, vy);
	int incremento = 1;
	float bola_x = 50;
	float bola_y = 0;
	float barra_x = largura_barra;
	float barra_y = (A - altura_barra)/2;
	bool cima = false;
	bool baixo = false;
	
	fila_allegro(1);
	al_start_timer(timer);
	bool jogando = true;

	while (jogando)
	{
		fila_allegro(2);	
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN) // Alguma tecla apertada
		{
			switch (evento.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				cima = true;
				break;
			case ALLEGRO_KEY_DOWN:
				baixo = true;
				break;
			}
		}
		if (evento.type == ALLEGRO_EVENT_KEY_UP) // Alguma tecla apertada
		{
			switch (evento.keyboard.keycode)
			{
			case ALLEGRO_KEY_UP:
				cima = false;
				break;
			case ALLEGRO_KEY_DOWN:
				baixo = false;
				break;
			}
		}
		if ( cima && barra_y > 0)
			barra_y -= 3;
		if ( baixo && barra_y < A-altura_barra)
			barra_y += 3;
		if (evento.type == ALLEGRO_EVENT_TIMER)
			desenhe_tela = true;
		if ( desenhe_tela && al_is_event_queue_empty(fila_eventos))
		{
			desenhe_tela = false;
			i += incremento;
			bola_x += vx;
			bola_y += vy;
			if (i > 245 || i < 1)
				incremento = -1 * incremento;
			if (bola_x > L - raio_bola)			// condicao lateral direita (parede)
				vx = -1.05 * vx;
			if (bola_y > A - raio_bola || bola_y < 0) // Paredes superiores e inferiores
				vy = -1 * vy;
			if (bola_x < barra_x + largura_barra )	// caso o eixo x da bolinha se encontre com a linha da barra
			{
				if ((bola_y + raio_bola) > barra_y && (bola_y + raio_bola ) < (barra_y + altura_barra))// entre as 2 barras
				{
					vx = -1 * vx;
				}			
				else
				{
					jogando = false;
				}			
			}
			al_clear_to_color(al_map_rgb(255-i, 0, i));
			al_draw_bitmap(bola, bola_x, bola_y, 0);
			al_draw_bitmap(barra, barra_x, barra_y, 0);
			al_flip_display();


			/*
			al_draw_scaled_bitmap(loaded_bmp,
			0, 0,                                // source origin
			al_get_bitmap_width(loaded_bmp),     // source width
			al_get_bitmap_height(loaded_bmp),    // source height
			0, 0,                                // target origin
			w, h,                                // target dimensions
			0                                    // flags
			);
			*/
		}
		if (tem_eventos)
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
		
	}
}

void tela_configuracao()
{

}

void escreve_texto_opcoes(vector<string> opcoes,int escolhido)
{
	int cor = 255;
	int numero_elementos = opcoes.size();
	for (int i = 0; i < opcoes.size(); i++)
	{
		if (i + 1 == escolhido)
			cor = 0;
		else
			cor = 255;
		al_draw_text(fonte, al_map_rgb(255, 255, cor), L/6,(i+1)*A/(numero_elementos+2), 
						ALLEGRO_ALIGN_LEFT, opcoes[i].c_str());
	}
}

void inicio_setup()
{
	std::vector<std::string> opcoes = { "Iniciar", "Configurar","Pong","Login"};
	int numero_opcoes = opcoes.size();
	fprintf(stderr, "n: %i", numero_opcoes);
	escreve_texto_opcoes(opcoes,1);
	al_play_sample(som_setup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &id);
	imagem = al_load_bitmap("imagens/vader02.JPG");
	al_draw_scaled_bitmap(imagem, 0, 0,
		al_get_bitmap_width(imagem), al_get_bitmap_height(imagem),
		0, 0,
		L, A, 0);
	escreve_texto_opcoes(opcoes, 1);
	int estado = 1;
	bool click = false;

	while ( !click )
	{
		fila_allegro(2);
		al_flip_display();
		if (tem_eventos)
		{
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;	// Sai quando apertar o "x" da janela
			else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) // Alguma tecla apertada
			{
				switch (evento.keyboard.keycode)
				{
				case ALLEGRO_KEY_UP:
					estado--;
					break;
				case ALLEGRO_KEY_DOWN:
					estado++;
					break;
				case ALLEGRO_KEY_ENTER:
					click = true;
					break;
				}
				if (estado > numero_opcoes)
					estado = 1;
				else if (estado < 1)
					estado = numero_opcoes;
				fprintf(stderr, "estado: %i \n", estado); // debug (retirar)
			}
			escreve_texto_opcoes(opcoes, estado);
			al_flip_display();
		}
	}
	if (click)
	{
		switch (estado)
		{
		case 1:
			jogo();
			break;
		case 2:
			tela_configuracao();
			break;
		case 3:
			jogo_pong();
			break;
		case 4:
			tela_login();
			break;
		}
	}
	al_stop_sample(&id);
}

void cutscene(int tipo)
{
	//al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());
	//al_set_audio_stream_playing(musica, true);
	int tempo_total;
	al_clear_to_color(al_map_rgb(0, 0, 0));
	if (tipo == 1)// abertura
	{
		al_play_sample(som_abertura, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
		imagem = al_load_bitmap("imagens/vader01.JPG");
		tempo_total = tempo_abertura;
	}
	else if (tipo == 2)// fim de jogo
	{
		al_play_sample(som_fim, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
		imagem = al_load_bitmap("imagens/lose01.JPG");
		tempo_total = tempo_final;
	}
	else
	{
		return;
	}
	int frames = 0;
	while (frames < tempo_total*fps)
	{
		fila_allegro(2);
		al_draw_scaled_bitmap(imagem, 0, 0,
			al_get_bitmap_width(imagem), al_get_bitmap_height(imagem),
			0, 0,
			L, A, 0);
		al_flip_display();
		if (tem_eventos)
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;	// Sai quando apertar o "x" da janela
		//fprintf(stderr, "frame %i \n", frames);
		frames++;
	}
	al_stop_sample(&id);
}

int fila_allegro(int tipo) 
{
	al_register_event_source(fila_eventos, al_get_display_event_source(janela));
	al_register_event_source(fila_eventos, al_get_keyboard_event_source());
	if (tipo == 1)
	{
		al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
	}
	else if (tipo == 2)
	{
		al_init_timeout(&timeout, 1.0 / fps);
		tem_eventos = al_wait_for_event_until(fila_eventos, &evento, &timeout);
	}
	return 1;
}

int inicializar_allegro()
{
	if (!al_init())
	{
		fprintf(stderr, "Falha ao inicializar a biblioteca Allegro.\n");
		return false;
	}

	timer = al_create_timer(1.0 / fps);
	if (!timer)
	{
		fprintf(stderr, "Erro no timer do allegro.\n");
		return false;
	}

	if (!al_install_keyboard())
	{
		fprintf(stderr, "Falha ao inicializar o teclado.\n");
		return false;
	}

	al_init_font_addon();
	if (!al_init_ttf_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on allegro_ttf.\n");
		return -1;
	}

	fonte = al_load_font("pirulen.ttf", 26, 0);
	if (!fonte)
	{
		al_destroy_display(janela);
		fprintf(stderr, "Falha ao carregar fonte.\n");
		return -1;
	}

	if (!al_install_audio()) {
		fprintf(stderr, "failed to initialize audio!\n");
		return -1;
	}

	if (!al_init_acodec_addon()) {
		fprintf(stderr, "failed to initialize audio codecs!\n");
		return -1;
	}

	if (!al_reserve_samples(3)) {
		fprintf(stderr, "failed to reserve samples!\n");
		return -1;
	}

	som_setup = al_load_sample("sons/respira.wav");

	if (!som_setup) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}

	som_fim = al_load_sample("sons/starwars.wav");

	if (!som_fim) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}

	som_abertura = al_load_sample("sons/imperial.wav");

	if (!som_abertura) {
		printf("Audio clip sample not loaded!\n");
		return -1;
	}

	musica = al_load_audio_stream("sons/imperial.wav", 4, 1024);
	if (!musica)
	{
		fprintf(stderr, "Falha ao carregar audio.\n");
		al_destroy_event_queue(fila_eventos);
		al_destroy_display(janela);
		return false;
	}

	if (!al_init_primitives_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on de primitivas.\n");
		return false;
	}

	if (!al_init_image_addon())
	{
		fprintf(stderr, "Falha ao inicializar add-on de imagens.\n");
		return false;
	}

	janela = al_create_display(L, A);
	if (!janela)
	{
		fprintf(stderr, "Falha ao criar janela.\n");
		return false;
	}

	fila_eventos = al_create_event_queue();
	if (!fila_eventos)
	{
		fprintf(stderr, "Falha ao criar fila de eventos.\n");
		al_destroy_display(janela);
		return -1;
	}
	al_set_window_title(janela, "Testando allegro_primitives");

	bola = al_load_bitmap("imagens/bola.png");
	barra = al_load_bitmap("imagens/barra.png");

	return true;
}

void the_end_allegro()
{
	al_destroy_audio_stream(musica);
	al_destroy_sample(som_abertura);
	al_destroy_sample(som_setup);
	al_destroy_sample(som_fim);
	al_destroy_display(janela);
	al_destroy_event_queue(fila_eventos);
}

