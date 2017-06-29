#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>

using namespace std;

typedef struct sep_instr{
      string rotulo,op,mem1,mem2;
      void reset() {
        rotulo.clear();
        op.clear();
        mem1.clear();
        mem2.clear();
    }
} sep_instr;

typedef struct st_tab_simb{
      string rotulo,value;
      int ender,rot_extern,bss_data;
} st_tab_simb;


struct sep_instr separa_linha(string);
int procura_ts(vector<st_tab_simb>, string, int*);

int main(int argc,char *argv[]){

	//*************************************//
	//****     DECLARACAO VARIAVEIS   ****//	
	string line,nome_saida,rotulo,op,mem1;
	sep_instr instruc;
	int conta_linha=1,existe_rotul;
	vector<st_tab_simb> tabela_simb;
	vector<st_tab_simb>::iterator tabela_simb_it;
	int *flag_EXTERN = (int*)calloc(1,sizeof(int));
	st_tab_simb simbolo1;


	// ****** CHECA NUMERO DE PARAMETROS ******//
	if(argc<3){
		cout<<"Parametros de entrada devem ser 2:\n arquivo.asm arquivo.s\n";
		exit(1);
	}

	/*** ARQUIVOS   ***/
  	ifstream myfile (argv[1]);  //abre arquivo fonte
  	nome_saida = argv[2];
 	nome_saida = nome_saida + ".o";
  	ofstream fp_fileout (nome_saida.c_str());  	//abre arquivo destino

  	//verifica se arquivos foram abertos
	if (!fp_fileout.is_open() || ! myfile.is_open()){  
	cout<<"arquivo não abriu! "<< endl;
	return 1;
	}

	while(getline(myfile,line)){

		instruc = separa_linha(line);

		rotulo = instruc.rotulo;
		op = instruc.op;
		mem1 = instruc.mem1;

		if(!rotulo.empty()){
      		/***procura por rotulo na TS**/
      		existe_rotul = procura_ts(tabela_simb,rotulo,flag_EXTERN);


      		if(existe_rotul > 0){
        		cout<<"error semantico, rotulo \":"<<rotulo<<"\" redefinido"<<endl;
        		cout<<"linha "<<conta_linha<<": "<<line<<endl;
        		exit(1);
      		}
      		else{
        		simbolo1.rotulo = rotulo;
        		simbolo1.ender = conta_linha; //mudei aqui rapidão.
        		simbolo1.rot_extern = 0;
        		if(op=="EXTERN"){
          		simbolo1.rot_extern =1;
        		}
        		if(op == "SPACE"){
        			simbolo1.bss_data = 1; //declarar no .bss
        			simbolo1.value = '1';
        			if(!mem1.empty()){
        				simbolo1.value = mem1 ;//atoi(mem1.c_str())
        			}
        		}else if(op == "CONST"){ 
        			simbolo1.bss_data = 2; // declarado no .Data
        			simbolo1.value = mem1;
        		} else{
        			simbolo1.bss_data = -2; //uma label
        			simbolo1.value = '0'; // Label
        		}

        		tabela_simb.push_back(simbolo1);    //insere rotulo e posição  na tabela de simbolos
        
      		}//if(existe_rotul)/else
    	}//if(!rotulo.empty())
    	conta_linha++;
	} //while(getline)


	fp_fileout<<"section .data"<<endl;
	cout<<"SIMBOL TABLE: "<<endl;
  	for(tabela_simb_it = tabela_simb.begin(); tabela_simb_it!=tabela_simb.end(); tabela_simb_it++){
      simbolo1 = *tabela_simb_it;
      cout<<simbolo1.rotulo<<" "<<simbolo1.ender<<" "<<simbolo1.bss_data<<" ";
      cout<<simbolo1.value<<endl;
      if(simbolo1.bss_data==2){
      	fp_fileout<<"\t"<<simbolo1.rotulo<<"  dw   "<<simbolo1.value<<endl;
      }
  	}
  	fp_fileout<<"section .bss"<<endl;
  	for(tabela_simb_it = tabela_simb.begin(); tabela_simb_it!=tabela_simb.end(); tabela_simb_it++){
      simbolo1 = *tabela_simb_it;
      if(simbolo1.bss_data==1){
      	fp_fileout<<"\t"<<simbolo1.rotulo<<"  resw   "<<simbolo1.value<<endl;
      }
  	}

	return 0;
}

struct sep_instr separa_linha(string line){

  struct sep_instr instruc;

  int pos;

   pos = (int)line.find(":");
    if(pos > 0){
      instruc.rotulo.assign(line,0,pos);
      line.erase(0,pos+1); //eliminar também o espaço após ':'
    }

    /**PROCURA OPERAÇÃO**/
    pos = (int)line.find(" "); //espaço para pegar codigo de instrução
    if(pos>0){
      instruc.op.assign(line,0,pos);  // pega codigo de instrução
      line.erase(0,pos+1); //+1 para eliminar espaço
    }else{ //if(op.empty()){
      /*caso do STOP e SPACE*/
      instruc.op=line;
      line.erase(0,line.length());
      //return caso faça função 
    }

    /** PRIMEIRO OPERANDO **/
    pos = (int)line.find(",");
    if(pos>0 ){
      instruc.mem1.assign(line,0,pos);
      line.erase(0,pos+1); //ja elimina o ;
      if(line.length()>0){  //segundo operando;
        instruc.mem2 = line;
        line.erase(0,instruc.mem2.length());
      }
    }
    else if(line.length()>0){
      pos = (int)line.find(" ");
      if(pos<0){
        instruc.mem1 = line;
        line.erase(0,pos);
      }else{
        instruc.mem1.assign(line,0,pos);
        line.erase(0,pos+1); //ja elimina o " "
        instruc.mem2.assign(line,0,pos);
        line.erase(0,pos);
      }
    }

  return instruc;
}

int procura_ts(vector<st_tab_simb> tabela_simb, string rotulo,int* flag_EXTERN){

  
  st_tab_simb simbolo1;
  int existe_rotul =-1;
  vector<st_tab_simb>::iterator tabela_simb_it;
    for(tabela_simb_it = tabela_simb.begin(); tabela_simb_it!=tabela_simb.end(); tabela_simb_it++){
      simbolo1 = *tabela_simb_it;
      if(rotulo == simbolo1.rotulo){
        existe_rotul = simbolo1.ender;
        *flag_EXTERN = simbolo1.rot_extern;
      }
    }
    return existe_rotul;
}
