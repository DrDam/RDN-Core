#include "Neurone.h"
#include "Synapse.h"
#include <vector>
#include "math.h"

int Neurone::m_seuil = 100; // prametre du programme => static/global ?
int Neurone::m_seuil_bas = 10; // prametre du programme => static/global ?
int Neurone::m_taux_stab = 10;  // % prametre du programme => static/global ?
int Neurone::m_base_pps = 5 ;  //% prametre du programme => static/global ?

Neurone::Neurone(int id) : m_id(id), m_actif(0), m_potentiel(Neurone::m_seuil_bas){ // créer un neurone

}
Neurone::Neurone(int id, int x, int y, int z) : m_id(id), m_actif(0), m_potentiel(Neurone::m_seuil_bas), m_x_coord(x), m_y_coord(y), m_z_coord(z) {
}

void Neurone::setstatic( int setseuil, int setbas, int settaux, int setpss) {
    Neurone::m_seuil = setseuil;
    Neurone::m_seuil_bas = setbas;
    Neurone::m_taux_stab = settaux ;
    Neurone::m_base_pps = setpss ;
}


//=> destructeur ( un neurone "disparait" s'il n'a plus de synapse amont ou aval => a spécifier clairement)
    //=> détruit les synapses restantes
Neurone::~Neurone() {
    #pragma omp parallel for
    for( unsigned int i=0;i<T_amont.size();++i){ // pour toutes les synapses Amont
		 T_amont[i]->detruit( false ) ; // on les détruit en précisant que l'ordre vient du neurone "aval"
		}
    #pragma omp parallel for
	for( unsigned int i=0;i<T_aval.size();++i){ // pour toutes les synapses aval
		 T_aval[i]->detruit( true ) ;// on les détruit en précisant que l'ordre vient du neurone "amont"
		} }


//=> PPS : ajoute un PPS au potentiel du neurone
void Neurone::PPS( int type, int coef) {
    if(m_actif == true) { // si le neurone est actif, on fait évoluer le pps
        float calc=(m_base_pps + type);
        calc=calc/m_base_pps;
        m_potentiel*=pow(calc,coef);
    }
}
//=> exemple base_pps = 5 et type = +1 => self:potentiel = self:potentiel * 6/5


//=>Stabilise : permet au neurone de retrouver sont potentiel de base
void Neurone::stabilise() {
if(m_potentiel<=m_seuil_bas)  // si le potentiel est inférieur au potentiel de base => i.e. le neurone est reposé
    {
    m_actif = true; // le neurone redevient actif
    m_potentiel = m_seuil_bas; // on bloque le potentiel au seuil bas
    }
    else
    {
        float calc=(100-m_taux_stab);
        calc = calc/100;
         m_potentiel *= calc; // sinon on diminu le potentiel
    }

}
//=> exemple taux_stab = 4/5 => self:potentiel = self:potentiel * 4/5


//=> ajoute une synapse à un neurone
void Neurone::ajout_synapse(Synapse* synapse , bool synapse_amont) {
    //std::cout << "ajout synapse" << std::endl;
    if ( synapse_amont == true ) { T_amont.push_back( synapse ); }
	else { T_aval.push_back( synapse ); }
}



//=> détruit une référence à une synapse
void Neurone::detruire_synapse(Synapse* synapse_cible , bool amont ) {
    /*
    Si amont = VRAI
        Alors cible = self:Synapses_amont
        Sinon cible = self:Synapses_aval
    FinSi
    Pour chaque référence_synapse dans cible
        faire
            Si référence_synapse = synapse_cible
                Alors référence_synapse = null
            FinSi
    FinPour
    */
}

//=> fonctionnement normal du neurone
void Neurone::run() {
    stabilise() ; // abaise le potentiel
    if (m_potentiel>m_seuil && m_actif==1)// si le potentiel est toujours supérieur au seuil et le neurone est "actif"
        {
      #pragma omp parallel for
	  for( unsigned int i=0;i<T_aval.size();i++){ // on transmet le PA à toutes les synapses avals
            T_aval[i]->transmet(1) ;
            }
        m_actif = false; // le neurone rentre en phase réfractaire
        }

}

 // renvoi le potentiel du neurone
float Neurone::getpotentiel()
{
    return m_potentiel;
}

// Renvoi l'id du Neurone
int Neurone::getId(){ return m_id;}

int Neurone::get_x_coord(){ return m_x_coord;}
int Neurone::get_y_coord(){ return m_y_coord;}
int Neurone::get_z_coord(){ return m_z_coord;}



// Le neuron est-il actif
bool Neurone::is_active(){ return m_actif;}

// Retourne toutes les synapses amont
std::vector<Synapse*> Neurone::getAllAmont()
 {
    return T_amont;
 }

// Retourne toutes les synapses avals
std::vector<Synapse*> Neurone::getAllAval()
 {
    return T_aval;
 }
