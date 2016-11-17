/*
	Gustavo Voltani von Atzingen agosto 2016

	Jogo para aprendizado do allegro e c++

	comentarui de teste 
*/


// Coisas do c++
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <time.h>
#include "string"
#include <math.h>

#define M_PI 3.14159
// Includes do alegro
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using namespace std;

// Minhas funções e macros
void escreve_texto(vector<string> opcoes, int escolhido, int tipo);
int inicializar_allegro(void);
void inicia_script(string arquivo);
void the_end_allegro(void);
int fila_allegro(int tipo);
void cutscene(int tipo);
void inicio_setup(void);
void jogo(void);
void tela_configuracao(void);
void tela_login(void);
void jogo_pong(void);
bool sabre_defesa(float targuet_x, float targuet_y, float sabre_x, float sabre_y, float sabre_theta);
float dist_euclidiana(float p1_x, float p1_y,float p2_x, float p2_y);
void escreve_arquivo(string conteudo, string nome_arquivo);

// Variáveis/structs públicas
int L = 1200;
int A = 800;
bool full_screen = false;
float fps = 60;
ALLEGRO_DISPLAY *janela = NULL;
ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
ALLEGRO_EVENT evento;
ALLEGRO_TIMEOUT timeout;
ALLEGRO_BITMAP *imagem = NULL;
ALLEGRO_BITMAP *fundo_jogo = NULL;
ALLEGRO_BITMAP *bola = NULL;
ALLEGRO_BITMAP *barra = NULL;
ALLEGRO_BITMAP *sabre = NULL;
ALLEGRO_BITMAP *remote = NULL;
ALLEGRO_AUDIO_STREAM *musica = NULL;		// TODO
ALLEGRO_SAMPLE *som_setup;
ALLEGRO_SAMPLE *som_fim;
ALLEGRO_SAMPLE *som_saber_on;
ALLEGRO_SAMPLE *som_perdeu;
ALLEGRO_SAMPLE *som_bolinha_parede;
ALLEGRO_SAMPLE *som_tiro;
ALLEGRO_SAMPLE *som_sabre_defesa;
ALLEGRO_SAMPLE *toma_tiro;
ALLEGRO_SAMPLE *som_bolinha_barra;
ALLEGRO_SAMPLE *som_aplauso;
ALLEGRO_SAMPLE *som_abertura;
ALLEGRO_SAMPLE_ID id;
ALLEGRO_FONT *fonte = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_MOUSE_STATE mouse_state;
ALLEGRO_DISPLAY_MODE   disp_data;

int tem_eventos;
float tempo_abertura = 10.0;
float tempo_final = 3.0;

int main(int argc, char **argv)
{
	srand(time(NULL));
	if (inicializar_allegro())
	{
		cutscene(1);
		inicio_setup();
		cutscene(3);
	}
	the_end_allegro();
	return 0;
}

void jogo()
{
	al_stop_sample(&id);
	int valor_lido = 1;
	string line;
	ifstream myfile("scripts/tipo_controle.txt");
	if (myfile.is_open())
	{
		getline(myfile, line);
		myfile.close();
		valor_lido = (int)line[0] - 48;
		fprintf(stderr, "valor lido: %i \n", valor_lido);
	}
	if (valor_lido == 2)
	{
		inicia_script("run_mouse.bat");
	}
	else if (valor_lido == 3)
	{
		inicia_script("run_sabre_camera.bat");
	}
	fprintf(stderr, "entrou no jogo \n");
	float remote_largura = al_get_bitmap_width(remote);
	float remote_altura = al_get_bitmap_height(remote);
	float sabre_altura = al_get_bitmap_height(sabre);
	float sabre_largura = al_get_bitmap_width(sabre);
	float sabre_x = 2.0*L / 3.0;
	float sabre_y = A / 2.0;
	float sabre_vx = 5.0;
	float sabre_vy = 5.0;
	float sabre_vtheta = 2.0;
	bool sabre_ligado = false;
	float remote_x = A / 2.0;
	float remote_y = A / 5.0;
	float remote_vx = 2;
	float remote_vy = 2;
	float sabre_theta = 90.0;
	float sabre_phi = 0.0;
	float sabre_escala = (A / sabre_altura) / 6.0;
	float laser_vel = 0.01;
	float laser_t_min = 3.0;
	float laser_t_max = 5.0;
	float laser_t_ultimo = 0.0;
	float laser_t_proximo = 10000.0;
	bool laser_atirar = false;
	bool laser_randon_time = false;
	int max_tiros_tela = 3;
	bool cima = false;
	bool baixo = false;
	bool esquerda = false;
	bool direita = false;
	bool theta_direita = false;
	bool theta_esquerda = false;
	bool desenhe_tela = false;
	int n_tiros = -1;
	int vida = 100;
	float pos_tiros[7][35];
	int contador_zica = 0;
	fprintf(stderr, "AL\n");
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 35; j++)
		{
			pos_tiros[i][j] = -1.0;
		}
	}
	fprintf(stderr, "dl\n");
	int pontos = 0;
	int frame = 0;
	int fps_tela = 0;
	int tempo = al_get_time();
	fprintf(stderr, "A\n");
	fila_allegro(1);
	al_start_timer(timer);
	fprintf(stderr, "B\n");
	bool jogando = true;
	while (jogando)
	{
		if (vida <= 0)
			jogando = false;
		al_wait_for_event(fila_eventos, &evento);
		if (tem_eventos)
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
		if (evento.type == ALLEGRO_EVENT_KEY_DOWN) // Alguma tecla apertada
		{
			switch (evento.keyboard.keycode)
			{
			case ALLEGRO_KEY_L:
				sabre_ligado = !sabre_ligado;
				al_play_sample(som_saber_on, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
				break;
			case ALLEGRO_KEY_A:
				esquerda = true;
				break;
			case ALLEGRO_KEY_D:
				direita = true;
				break;
			case ALLEGRO_KEY_W:
				cima = true;
				break;
			case ALLEGRO_KEY_S:
				baixo = true;
				break;
			case ALLEGRO_KEY_DOWN:
				//baixo = true;
				break;
			case ALLEGRO_KEY_UP:
				//cima = true;
				break;
			case ALLEGRO_KEY_LEFT:
				theta_esquerda = true;
				break;
			case ALLEGRO_KEY_RIGHT:
				theta_direita = true;
				break;
			case ALLEGRO_KEY_ESCAPE:
				jogando = false;
			default:
				break;
			}
		}
		if (evento.type == ALLEGRO_EVENT_KEY_UP) // Alguma tecla apertada
		{
			switch (evento.keyboard.keycode)
			{
			case ALLEGRO_KEY_A:
				esquerda = false;
				break;
			case ALLEGRO_KEY_D:
				direita = false;
				break;
			case ALLEGRO_KEY_W:
				cima = false;
				break;
			case ALLEGRO_KEY_S:
				baixo = false;
				break;
			case ALLEGRO_KEY_DOWN:
				//baixo = true;
				break;
			case ALLEGRO_KEY_UP:
				//cima = true;
				break;
			case ALLEGRO_KEY_LEFT:
				theta_esquerda = false;
				break;
			case ALLEGRO_KEY_RIGHT:
				theta_direita = false;
				break;
			default:
				break;
			}
		}
		if (evento.type == ALLEGRO_EVENT_TIMER)
		{
			desenhe_tela = true;
			if (theta_esquerda)
				sabre_theta += 1;
			if (theta_direita)
				sabre_theta -= 1;
			if (direita)
				sabre_x += sabre_vx;
			if (esquerda)
				sabre_x -= sabre_vx;
			if (cima)
				sabre_y -= sabre_vy;
			if (baixo)
				sabre_y += sabre_vy;
			al_get_mouse_state(&mouse_state);
			sabre_theta = mouse_state.x*(360) / L + 90;
			if (remote_x > (3.0*L / 4.0) || remote_x < L / 3.0)
				remote_vx = -1 * remote_vx;
			else if (rand() % 100  > 98)
				remote_vx = -1 * remote_vx;
			if (remote_y > 2.0*A / 5.0 || remote_y < 10)
				remote_vy = -1 * remote_vy;
			else if (rand() % 100 < 2)
				remote_vy = -1 * remote_vy;		
			if (!laser_randon_time && al_get_time() - laser_t_ultimo > laser_t_min )
			{
				laser_randon_time = true;
				laser_t_proximo = al_get_time() + ((rand()%100)/100.0)*(laser_t_max - laser_t_min) ;
			}
			else if (laser_randon_time)
			{
				if (al_get_time() > laser_t_proximo)
				{
					laser_atirar = true;
					laser_randon_time = false;
					laser_t_ultimo = al_get_time();
				}
			}
			if ( laser_atirar )
			{
				laser_atirar = false;
				fprintf(stderr, "novo tiro: ");
				al_play_sample(som_tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
				for (int i = 0; i < max_tiros_tela; i++)
				{
					if (pos_tiros[0][i] == -1)
					{
						pos_tiros[0][i] = remote_x;	// posicao inicial do tiro x
						pos_tiros[1][i] = remote_y;	// posicao inicial do tiro y
						pos_tiros[2][i] = remote_x;	// posicao atual do tiro x
						pos_tiros[3][i] = remote_y;	// posicao atual do tiro y
						int targuetX = remote_x;
						while ( ( targuetX > (remote_x - 50)) && ( targuetX < (remote_x + 50) ) )
							targuetX = L*((rand()%1001)/1000.0);
						pos_tiros[4][i] = targuetX;		// posicao aleatoria final do tiro x
						int targuetY = remote_y;
						while ((targuetY >(remote_y - 50)) && (targuetY < (remote_y + 50)))
							targuetY = A*((rand() % 1001) / 1000.0);
						pos_tiros[5][i] = targuetY;		// posicao aleatoria final do tiro y
						pos_tiros[6][i] = dist_euclidiana(pos_tiros[0][i], pos_tiros[1][i], pos_tiros[4][i], pos_tiros[5][i]);
						break;
					}
				}
			}
			// atualizando a posicao dos tiros:
			for (int i = 0; i < max_tiros_tela; i++)
			{
				if (pos_tiros[0][i] > -1)
				{
					float dist_alvo = dist_euclidiana(pos_tiros[2][i], pos_tiros[3][i], pos_tiros[4][i], pos_tiros[5][i]);
					if ( dist_alvo < 30)
					{
						if (sabre_defesa(pos_tiros[2][i], pos_tiros[3][i], sabre_x, sabre_y, sabre_theta))
						{
							al_play_sample(som_sabre_defesa, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
							pontos++;
							if (pontos % 10 == 0)
								if (vida <= 90)
									vida =+ 10;
						}
						else
						{
							al_play_sample(toma_tiro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
							vida = vida - 10;
						}
							
						for (int k = 0; k < 7; k++)
							pos_tiros[k][i] = -1;
					}
					else
					{
						float vel_perspectiva;
						vel_perspectiva = abs(pos_tiros[6][i] - dist_alvo);
						if (vel_perspectiva < 150) vel_perspectiva = 150;
						else if (vel_perspectiva > 250) vel_perspectiva = 250;
						pos_tiros[2][i] += vel_perspectiva * laser_vel * ((pos_tiros[4][i] - pos_tiros[0][i])/ dist_alvo);
						pos_tiros[3][i] += vel_perspectiva * laser_vel * ((pos_tiros[5][i] - pos_tiros[1][i])/ dist_alvo);
					}			
				}
			}
			remote_x += remote_vx;
			remote_y += remote_vy;
		}
		if (desenhe_tela && al_is_event_queue_empty(fila_eventos))
		{
			desenhe_tela = false;
			if (al_get_time() - tempo > 1)
			{
				tempo = al_get_time();
				fps_tela = frame;
				frame = 0;
			}
			al_draw_scaled_bitmap(fundo_jogo, 0, 0,
				al_get_bitmap_width(fundo_jogo), al_get_bitmap_height(fundo_jogo),
				0, 0,
				L, A, 0);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L / 6, 3.5 * A / 6.0, ALLEGRO_ALIGN_LEFT, "fase 1:  %i %%", pontos * 4);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L / 6, 4*A / 6.0, ALLEGRO_ALIGN_LEFT, "vida: %i %%", vida);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L / 6, 4.5*A / 6.0, ALLEGRO_ALIGN_LEFT, "pontos: %i", pontos);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L / 6, 5 * A / 6.0, ALLEGRO_ALIGN_LEFT, "tempo: %i", tempo);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L / 6, 5.5*A / 6.0, ALLEGRO_ALIGN_LEFT, "%ifps", fps_tela);
			
			al_draw_scaled_rotated_bitmap(	sabre,	
											sabre_largura/2.0,sabre_altura/2.0,
											sabre_x,sabre_y,
											sabre_escala, sabre_escala,
											M_PI*(sabre_theta + 85) / 180.0,0);
			if (sabre_ligado)
			{
				al_draw_line(sabre_x + sabre_escala * (sabre_altura / 2.1) * cos(M_PI*sabre_theta / 180.0), sabre_y + sabre_escala * (sabre_altura / 2.1) * sin(M_PI*sabre_theta / 180.0),
					sabre_x + 500 * cos(M_PI*sabre_theta / 180.0), sabre_y + 500 * sin(M_PI*sabre_theta / 180.0),
					al_map_rgb(255, 0, 0), 10);
			}
			float escala = remote_y / 400;
			al_draw_filled_ellipse(remote_x + 10, 400,			// 10 Inserido manualmente (trocar)
									escala*18,escala*8,
									al_map_rgb(40,40,40));
			al_draw_tinted_scaled_bitmap(remote,
				al_map_rgb(100,100,100),
				0, 0,							
				remote_altura, remote_largura,	// source width, source height
				remote_x, remote_y,				// target origin
				35, 35,							// target dimensions
				0);		
			for (int i = 0; i < max_tiros_tela; i++)
			{
				if (pos_tiros[2][i] > -1)
				{
					float dist_alvo = dist_euclidiana(pos_tiros[2][i], pos_tiros[3][i], pos_tiros[4][i], pos_tiros[5][i]);
					float raio_bola = (600.0 / dist_alvo);
					if (raio_bola > 20) raio_bola = 20;
					al_draw_ellipse(pos_tiros[2][i], pos_tiros[3][i], raio_bola, raio_bola, al_map_rgb(255, 0, 0),5);
					al_draw_ellipse(pos_tiros[4][i], pos_tiros[5][i], 20, 20, al_map_rgb(0, 255, 0), 5);
				}
			}
			al_flip_display();
			frame++;
		}
	}
}

bool sabre_defesa(float x0, float y0, float x1, float y1, float s_theta)
{
	float x2 = x1 + cos(M_PI*s_theta / 180.0);
	float y2 = y1 + sin(M_PI*s_theta / 180.0);
	float distancia = abs((y2 - y1)*x0 - (x2 - x1)*y0 + x2*y1 - y2*x1) / sqrt(pow((y2 - y1), 2) + pow((x2 - x1), 2));
	fprintf(stderr, "distancia da defesa: %f, resultado %i \n", distancia, distancia < 30);
	if (distancia < 30)
		return true;
	else
		return false;
}

void tela_login()
{

}

void inicia_script(string arquivo)
{
	std::string command = "start \"teste\" scripts\\" + arquivo;
	system(command.c_str());
}

float dist_euclidiana(float p1_x, float p1_y, float p2_x, float p2_y)
{
	return sqrt(pow(p1_x - p2_x, 2) + pow(p1_y - p2_y, 2));
}

void jogo_pong()
{
	float bola_diametro_largura = al_get_bitmap_width(bola);
	float bola_diametro_altura = al_get_bitmap_height(bola);
	float barra_largura_original = al_get_bitmap_width(barra);
	float barra_altura_original = al_get_bitmap_height(barra);
	float bola_raio = A / 10;
	float barra_largura = L / 50.0;
	float barra_altura = L / 5.0;
	bool desenhe_tela = false;
	float vx = rand() / RAND_MAX + 4.5;   // |v| = 10
	float vy = sqrt(100 - vx*vx);
	float barra_velocidade = 6;
	float bola_x = L / 2.0;
	float bola_y = A / 2.0;
	float barra_x = L / 30.0;
	float barra_y = A/2 - (barra_altura/2);
	bool cima = false;
	bool baixo = false;

	fila_allegro(1);
	al_start_timer(timer);
	int pontos = 0;
	int frame = 0;
	int fps_tela = 0;
	int tempo = al_get_time();
	bool jogando = true;
	int c = 0;				// cor da tela (fundo)

	while (jogando)
	{
		al_wait_for_event(fila_eventos, &evento);
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
			case ALLEGRO_KEY_ESCAPE:
				jogando = false;
				break;
			default:
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
			default:
				break;
			}
		}
		if ( cima && barra_y > 0)
			barra_y -= barra_velocidade;
		if ( baixo && barra_y + barra_altura < A)
			barra_y += barra_velocidade;
		if (evento.type == ALLEGRO_EVENT_TIMER)
			desenhe_tela = true;
		if ( desenhe_tela && al_is_event_queue_empty(fila_eventos))
		{
			desenhe_tela = false;
			bola_x += vx;
			bola_y += vy;
			if (bola_x + bola_raio > L)						// condicao lateral direita (parede)
			{
				al_play_sample(som_bolinha_parede, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
				vx = -1.0 * vx;
				pontos++;
				if (pontos % 3 == 0)
				{
					al_play_sample(som_aplauso, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
					vx = 1.2*vx;
					bola_raio = bola_raio / 1.2;
					barra_altura = barra_altura / 1.2;
					barra_velocidade = 1.2*barra_velocidade;
					c += 20;
					if (c > 250)		// fim do jogo
						break;
				}
			}
			if (bola_y + bola_raio > A || bola_y < 0)			// Paredes superiores e inferiores
			{
				vy = -1 * vy;
				al_play_sample(som_bolinha_parede, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
			}
			if ( bola_x < barra_x + barra_largura)					// caso o eixo x da bolinha se encontre com a linha da barra
			{
				if (bola_y + bola_raio > barra_y && bola_y + bola_raio < barra_altura + barra_y)// entre as 2 barras
				{
					vx = -1 * vx;
					al_play_sample(som_bolinha_barra, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
				}
				else
					jogando = false;		
			}
			if (al_get_time() - tempo > 1)
			{
				tempo = al_get_time();
				fps_tela = frame;
				frame = 0;
			}
			al_clear_to_color(al_map_rgb(255 - c, 0, c));
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L/6, 4*A/6.0, ALLEGRO_ALIGN_LEFT, "fase 1:  %i %%", pontos*4);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L/6, 4.5*A/6.0, ALLEGRO_ALIGN_LEFT, "pontos: %i", pontos);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L/6, 5*A/6.0,ALLEGRO_ALIGN_LEFT, "tempo: %i", tempo);
			al_draw_textf(fonte, al_map_rgb(255, 255, 0), 4.5*L/6, 5.5*A/6.0, ALLEGRO_ALIGN_LEFT, "%ifps", fps_tela);
			al_draw_scaled_bitmap(bola, 
									0, 0,
									bola_diametro_largura, bola_diametro_altura, 
									bola_x, bola_y,
									bola_raio, bola_raio,
									0);
			al_draw_scaled_bitmap(barra,
									0, 0,							// source origin ????   
									barra_largura_original, barra_altura_original,	// source width, source height
									barra_x, barra_y,				// target origin
									barra_largura, barra_altura,	// target dimensions
									0);								// flags
			al_flip_display();
			frame++;
		}
		if (tem_eventos)
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;
		
	}
}

void tela_configuracao()
{
	std::vector<std::string> opcoes = { "Tipo Entrada","Porta Serial", "Calibrar Gyro","Calibrar Camera","voltar" };
	std::vector<std::string> opcoes_secundarias = { "","","","",""};
	int numero_opcoes = opcoes.size();
	int numero_opcoes_secundarias = 5;
	fprintf(stderr, "n: %i", numero_opcoes);
	escreve_texto(opcoes, 1, 1);
	al_play_sample(som_setup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &id);
	imagem = al_load_bitmap("imagens/vader01.JPG");
	al_draw_scaled_bitmap(imagem, 0, 0,
		al_get_bitmap_width(imagem), al_get_bitmap_height(imagem),
		0, 0,
		L, A, 0);
	escreve_texto(opcoes, 1, 1);
	int estado = 1;
	int estado_secundario = 1;
	bool principal = true;
	bool click = false;
	bool sair = false;

	while (!sair)
	{
		fila_allegro(2);
		al_flip_display();
		if (tem_eventos)
		{
			if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) break;	// Sai quando apertar o "x" da janela
			else if (evento.type == ALLEGRO_EVENT_KEY_DOWN) // Alguma tecla apertada
			{
				if (principal)
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
					case ALLEGRO_KEY_RIGHT:
						principal = false;
						break;
					}
					if (estado > numero_opcoes)
						estado = 1;
					else if (estado < 1)
						estado = numero_opcoes;
				}
				else
				{
					switch (evento.keyboard.keycode)
					{
					case ALLEGRO_KEY_UP:
						estado_secundario--;
						break;
					case ALLEGRO_KEY_DOWN:
						estado_secundario++;
						break;
					case ALLEGRO_KEY_LEFT:
						principal = true;
						break;
					case ALLEGRO_KEY_ENTER:
						click = true;
						break;
					}
					if (estado_secundario > numero_opcoes_secundarias)
						estado_secundario = 1;
					else if (estado_secundario < 1)
						estado_secundario = numero_opcoes_secundarias;
				}
				switch (estado)
				{
				case 1:
					opcoes_secundarias[0] = "Mouse";
					opcoes_secundarias[1] = "Serial";
					opcoes_secundarias[2] = "Camera";
					opcoes_secundarias[3] = "-----";
					opcoes_secundarias[4] = "-----";
					break;
				case 2:
					opcoes_secundarias[0] = "COM1";
					opcoes_secundarias[1] = "COM2";
					opcoes_secundarias[2] = "COM3";
					opcoes_secundarias[3] = "COM4";
					opcoes_secundarias[4] = "COM5";
					break;
				case 3:
					opcoes_secundarias[0] = "YAW";
					opcoes_secundarias[1] = "PITCH";
					opcoes_secundarias[2] = "ROLL";
					opcoes_secundarias[3] = "----";
					opcoes_secundarias[4] = "----";
					break;
				case 4:
					opcoes_secundarias[0] = "----";
					opcoes_secundarias[1] = "----";
					opcoes_secundarias[2] = "----";
					opcoes_secundarias[3] = "----";
					opcoes_secundarias[4] = "----";
					break;
				case 5:
					opcoes_secundarias[1] = "----";
					opcoes_secundarias[2] = "----";
					opcoes_secundarias[2] = "----";
					opcoes_secundarias[3] = "----";
					opcoes_secundarias[4] = "----";
					break;
				default:
					break;
				}
			}
			if (click)
			{
				click = false;
				if (principal)
				{
					switch (estado)
					{
					case 4:
						inicia_script("run_camera_config.bat");
						sair = true;
						break;
					case 5:
						sair = true;
						break;
					default:
						principal = false;
						break;
					}
				}
				else
				{
					switch (estado)
					{
					case 1:
						if (estado_secundario == 1) escreve_arquivo("1", "scripts/tipo_controle.txt");
						else if (estado_secundario == 2) escreve_arquivo("2", "scripts/tipo_controle.txt");
						else escreve_arquivo("3", "scripts/tipo_controle.txt");
						sair = true;
						break;
					default:
						principal = false;
						break;
					}
				}
			}
			al_draw_scaled_bitmap(imagem, 0, 0,
				al_get_bitmap_width(imagem), al_get_bitmap_height(imagem),
				0, 0,
				L, A, 0);
			escreve_texto(opcoes, estado, 1);
			escreve_texto(opcoes_secundarias, estado_secundario, 2);
			al_flip_display();
		}
	}
}

void escreve_arquivo(string conteudo, string nome_arquivo)
{
	ofstream arquivo_txt(nome_arquivo);
	if (arquivo_txt.is_open())
	{
		arquivo_txt << conteudo;
		arquivo_txt.close();
	}
	else
	{
		fprintf(stderr, "erro ao escrever no arquivo \n");
	}
}


void escreve_texto(vector<string> opcoes, int escolhido, int tipo)
{
	int cor = 255;
	int numero_elementos = opcoes.size();
	for (int i = 0; i < opcoes.size(); i++)
	{
		if (i + 1 == escolhido)
			cor = 0;
		else
			cor = 255;
		if ( tipo == 1)
			al_draw_text(fonte, al_map_rgb(255, 255, cor), L / 6, (i + 1)*A / (numero_elementos + 2),
			ALLEGRO_ALIGN_LEFT, opcoes[i].c_str());
		else if (tipo == 2)
			al_draw_text(fonte, al_map_rgb(255, 255, cor), 4*L / 6, (i + 1)*A / (numero_elementos + 2),
				ALLEGRO_ALIGN_LEFT, opcoes[i].c_str());
	}
}

void inicio_setup()
{
	std::vector<std::string> opcoes = { "Iniciar", "Configurar","Pong","Login","fechar"};
	int numero_opcoes = opcoes.size();
	fprintf(stderr, "n: %i", numero_opcoes);
	escreve_texto(opcoes,1,1);
	al_play_sample(som_setup, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &id);
	imagem = al_load_bitmap("imagens/vader02.JPG");
	al_draw_scaled_bitmap(imagem, 0, 0,
		al_get_bitmap_width(imagem), al_get_bitmap_height(imagem),
		0, 0,
		L, A, 0);
	escreve_texto(opcoes, 1, 1);
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
			escreve_texto(opcoes, estado, 1);
			al_flip_display();
		}
	}
	if (click)
	{
		switch (estado)
		{
		case 1:
			jogo();
			cutscene(2);
			break;
		case 2:
			tela_configuracao();
			break;
		case 3:
			jogo_pong();
			cutscene(2);
			al_stop_sample(&id);
			break;
		case 4:
			tela_login();
			break;
		default:
			break;
		}
		inicio_setup();
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
	else if (tipo == 2)// fim de jogo - perdeu (pong)
	{
		al_play_sample(som_perdeu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
		imagem = al_load_bitmap("imagens/lose01.JPG");
		tempo_total = tempo_final;
	}
	else if (tipo == 3)// fechando o programa
	{
		al_play_sample(som_fim, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &id);
		imagem = al_load_bitmap("imagens/lose02.JPG");
		tempo_total = 2*tempo_final;
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
	al_register_event_source(fila_eventos, al_get_mouse_event_source());
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

	fonte = al_load_font("pirulen.ttf", 20, 0);
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

	if (!al_reserve_samples(7)) {
		fprintf(stderr, "failed to reserve samples!\n");
		return -1;
	}

	som_aplauso = al_load_sample("sons/aplauso.ogg");

	if (!som_aplauso) {
		printf("Audio clip sample not loaded! aplauso.ogg\n");
		return -1;
	}

	som_perdeu = al_load_sample("sons/youlose.ogg");

	if (!som_perdeu) {
		printf("Audio clip sample not loaded! perdeu.wav\n");
		return -1;
	}

	som_bolinha_parede = al_load_sample("sons/pinga_parede.wav");

	if (!som_bolinha_parede) {
		printf("Audio clip sample not loaded! pinga_parede.wav \n");
		return -1;
	}

	//som_saber_on
	som_saber_on = al_load_sample("sons/som_saber_on.wav");

	if (!som_saber_on) {
		printf("Audio clip sample not loaded! som_saber_on.wav \n");
		return -1;
	}

	//som_tiro
	som_tiro = al_load_sample("sons/som_tiro.wav");

	if (!som_tiro) {
		printf("Audio clip sample not loaded! som_tiro.wav \n");
		return -1;
	}

	//toma_tipo
	toma_tiro = al_load_sample("sons/toma_tiro.wav");

	if (!toma_tiro) {
		printf("Audio clip sample not loaded! toma_tiro.wav \n");
		return -1;
	}

	//som_sabre_defesa
	som_sabre_defesa = al_load_sample("sons/som_sabre_defesa.wav");

	if (!som_sabre_defesa) {
		printf("Audio clip sample not loaded! som_sabre_defesa.wav \n");
		return -1;
	}

	som_bolinha_barra = al_load_sample("sons/pinga_barra.wav");

	if (!som_bolinha_barra) {
		printf("Audio clip sample not loaded! pinga_barra.wav\n");
		return -1;
	}

	som_setup = al_load_sample("sons/respira.wav");

	if (!som_setup) {
		printf("Audio clip sample not loaded! respira.wav\n");
		return -1;
	}

	som_fim = al_load_sample("sons/starwars.wav");

	if (!som_fim) {
		printf("Audio clip sample not loaded! starwars.wav\n");
		return -1;
	}

	som_abertura = al_load_sample("sons/imperial.wav");

	if (!som_abertura) {
		printf("Audio clip sample not loaded! imperial.wav \n");
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
	if (full_screen)
	{
		al_set_new_display_flags(ALLEGRO_FULLSCREEN);
		al_get_display_mode(0, &disp_data);
		L = disp_data.width;
		A = disp_data.height;
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

	if (!al_install_mouse())
	{
		fprintf(stderr, "Falha ao inicializar o mouse.\n");
		al_destroy_display(janela);
		return -1;
	}

	if (!al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT))
	{
		fprintf(stderr, "Falha ao atribuir ponteiro do mouse.\n");
		al_destroy_display(janela);
		return -1;
	}

	bola = al_load_bitmap("imagens/bola.png");
	barra = al_load_bitmap("imagens/barra.png");
	fundo_jogo = al_load_bitmap("imagens/dagobah.jpeg");
	sabre = al_load_bitmap("imagens/sabre_luz.png");
	remote = al_load_bitmap("imagens/remote.png");

	if (!bola || !barra || !fundo_jogo || !sabre || !remote)
	{
		fprintf(stderr, "zica na imagem.\n");
		al_destroy_display(janela);
		return -1;
	}


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
	//exit;				// TODO - como fecha o programa ????
}

