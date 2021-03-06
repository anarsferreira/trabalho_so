#include "util.h"

object_t new_object (char name[10])
{
    if (!strcmp ("sandes", name))
        return (object_t) {
            "sandes", .peso=0.5, .raridade=10,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=3, .def_diff=0
        };

    if (!strcmp ("aspirina", name))
        return (object_t) {
            "aspirina", .peso=0.1, .raridade=20,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=1, .def_diff=0
        };

    if (!strcmp ("xarope", name))
        return (object_t) {
            "xarope", .peso=1, .raridade=4,
            .f_ataque=0, .f_defesa=0, .max_uso=1,
            .hp_diff=4, .def_diff=0
        };

    if (!strcmp ("faca", name))
        return (object_t) {
            "faca", .peso=2, .raridade=5,
            .f_ataque=5, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=0
        };

    if (!strcmp ("espada", name))
        return (object_t) {
            "espada", .peso=8, .raridade=3,
            .f_ataque=8, .f_defesa=2, .max_uso=0,
            .hp_diff=0, .def_diff=2
        };

    if (!strcmp ("granada", name))
        return (object_t) {
            "granada", .peso=1, .raridade=2,
            .f_ataque=30, .f_defesa=0, .max_uso=1,
            .hp_diff=-5, .def_diff=0
        };

    if (!strcmp ("escudo", name))
        return (object_t) {
            "escudo", .peso=4, .raridade=4,
            .f_ataque=5, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=5
        };

    if (!strcmp ("moeda", name))
        return (object_t) {
            "moeda", .peso=0.1, .raridade=5,
            .f_ataque=0, .f_defesa=0, .max_uso=0,
            .hp_diff=0, .def_diff=0
        };
}

void apanha_objecto (int i, pid_t pid)
{
	int u, p;
	user_t c;

	u = find_user_index (pid);
	p = find_user_playing_index (pid);
	c = find_user (pid);

	// copiar o objecto da sala para o utilizador correspondente
	// tanto na lista de utilizadores como da dos que estao a jogar
	user_list[u].saco[user_list[u].n_obj] = labirinto[c.lin][c.col].objectos[i];
	user_list[u].peso_saco += labirinto[c.lin][c.col].objectos[i].peso;

	users_playing[p].saco[users_playing[p].n_obj] = labirinto[c.lin][c.col].objectos[i];
	users_playing[p].peso_saco += labirinto[c.lin][c.col].objectos[i].peso;

	user_list[i].n_obj++; users_playing[i].n_obj++;

	remove_object_sala (i, c.lin, c.col);	
}

void remove_object_sala (int i, int lin, int col)
{
	int j;
	for (j = i; j < labirinto[lin][col].n_obj - 1; j++)
		labirinto[lin][col].objectos[j] = labirinto[lin][col].objectos[j+1];

	memset (&labirinto[lin][col].objectos[labirinto[lin][col].n_obj],
			0, sizeof (object_t));

	labirinto[lin][col].n_obj--;
}

void larga_objecto (int i, pid_t pid)
{
	user_t c;
	c = find_user (pid);

	if (labirinto[c.lin][c.col].n_obj == 5)
		remove_object_saco (i, pid);

	else {
		labirinto[c.lin][c.col].objectos[labirinto[c.lin][c.col].n_obj] =
			c.saco[i];
		labirinto[c.lin][c.col].n_obj++;

		remove_object_saco (i, pid);
	}
}

void remove_object_saco (int i, pid_t pid)
{
	int j, u, p;
	user_t c;
	u = find_user_index (pid);
	p = find_user_playing_index (pid);
	c = find_user (pid);

	user_list[u].peso_saco -= user_list[u].saco[i].peso;

	for (j = i; j < c.n_obj-1; j++)
		user_list[u].saco[j] = user_list[u].saco[j+1];
	
	memset (&user_list[u].saco[c.n_obj], 0, sizeof (object_t));

	users_playing[p].peso_saco -= users_playing[p].saco[i].peso;
	for (j = i; j < c.n_obj-1; j++)
		users_playing[p].saco[j] = users_playing[p].saco[j+1];

	memset (&users_playing[p].saco[c.n_obj], 0, sizeof (object_t));

	user_list[u].n_obj--; users_playing[p].n_obj--;
}

void usa_objecto (int i, pid_t pid)
{
	int u, p;
	u = find_user_index (pid);
	p = find_user_playing_index (pid);

	user_list[u].hp += user_list[u].saco[i].hp_diff;
	users_playing[p].hp += users_playing[p].saco[i].hp_diff;
	
	// garantir que o hp nao fica maior que hp_max 
	if (user_list[u].hp >= user_list[u].hp_max)
		user_list[u].hp = user_list[u].hp_max; 

	if (users_playing[p].hp >= users_playing[p].hp_max)
		users_playing[p].hp = users_playing[p].hp_max;

	// pode-se usar o objecto as vezes que quiser se max_suo==0
	if (user_list[u].saco[i].max_uso == 0)
		return;

	// case exceda o numero de usos remover do saco
	user_list[u].saco[i].n_uso++;
	users_playing[p].saco[i].n_uso++;

	if (user_list[u].saco[i].n_uso == user_list[u].saco[i].max_uso)
		remove_object_saco (i, user_list[u].client_pid);

	if (users_playing[p].saco[i].n_uso == users_playing[p].saco[i].max_uso)
		remove_object_saco (i, users_playing[p].client_pid);
}


int is_object_name (char name[10])
{
	if (!strcmp (name, "sandes")
		|| !strcmp (name, "aspirina")
		|| !strcmp (name, "xarope")
		|| !strcmp (name, "faca")
		|| !strcmp (name, "espada")
		|| !strcmp (name, "granada")
		|| !strcmp (name, "escudo")
	) return 1;

	return 0;
}
