#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <list>
#include <sstream>
#include <vector>
using namespace std;

//Função que preprocessa o arquivo de entrada

bool pre_proc(ifstream &srce_file, fstream &no_comments)
{

	bool flag=false;
	string lines, words, words2, words3, words4;
	stringstream temp, temp1, temp2, temp3, temp5;
	vector<string> equ_list, tokens;
	vector<string>::iterator equ_list_it, tokens_it;


	while(getline(srce_file,lines))
	{
		transform(lines.begin(),lines.end(), lines.begin(), ::toupper); //Coloca tudo em caixa alta
		if (lines.find(";") != string::npos)						// Retira comentários
		{
			temp2 << lines.substr(0,int(lines.find(";"))) << endl;	
		}
		else					//Se não houver comentários, escreve a linha
		{
			temp2 << lines << endl;
		}
		//cout << temp2.str();
		

		if (temp2.str().find("EQU") != string::npos)	//Acha linhas com a diretiva EQU
		{
			temp << temp2.str().substr(0,int(temp2.str().find(":")));	//Pega o label da linha com EQU
			equ_list.push_back(temp.str());		//Salva o label no vector
			temp.str("");						//Limpa a string temporária
			temp1 << temp2.str().substr((temp2.str().find("EQU")+4),temp2.str().find("\n")); //Pega o valor do EQU
			equ_list.push_back(temp1.str()); //Adiciona o valor do EQU na posição seguinte do label no vector
			temp1.str("");
		}
		else if(!equ_list.empty())			//Se a lista de EQU tiver algo, verifica se cada linha possui os labels já encontrados
		{
			for(equ_list_it = equ_list.begin();	equ_list_it != equ_list.end(); equ_list_it+=2) //Percorre de 2 em 2 pra procurar apenas os labels e não os valores
			{	
				if(lines.find(*equ_list_it) != string::npos)
				{
					temp3 << temp2.str().substr(0, temp2.str().find(*equ_list_it)); //Remove o nome do label da string
					temp2.str("");
					temp2 << temp3.str();
					temp3.str("");
					advance(equ_list_it,1);			//Substitui a label pelo valor definido na EQU
					temp2 << *equ_list_it;
					advance(equ_list_it,-1);		//Retorna o iterator para não quebrar o for e não dar segfault
				}
			}
		}
		string temp4 = temp2.str();					//Faz o casting de streamstring para uma string temporária
		istringstream is(temp4);
		while (getline(is,words, '\t'))				//Separa as linhas em tokens delimitados por \t ou ' '
		{
			istringstream is2(words);
			while (getline(is2, words2, ' '))
			{
				istringstream is4(words2);
				while(getline(is4,words4,'\t'))
					tokens.push_back(words4);			//Coloca os tokens em um vector
			}
		}

		for(unsigned int i = 0; i < tokens.size(); i++)	//Itera pelo vector de tokens
		{
			short int subcounter = i -1;								//Evita segfault
			if(subcounter < 0)
				subcounter = 0;

			if(tokens[i].find("IF") != string::npos && (tokens[i+1].find("1") == string::npos) && !flag)		//Se acha a diretiva IF e ela tem um valor 0 pela frente, seta a flag
			{
				flag = true;
				i++;
				continue;
			}
			else if(tokens[i].find("IF") != string::npos && (tokens[i+1].find("1") != string::npos) && flag)
			{
				flag = false;
				i++;
				continue;
			}									
			else if((tokens[i].find(':') != string::npos || tokens[i].find('\n') != string::npos) && tokens[i].find("IF") == string::npos && !flag) //Se for label ou token com quebra de linha, insere apenas o token
			{
				temp5 << tokens[i];
			}
			else if((tokens[i].find('\n') == string::npos) && tokens[i].find("IF") == string::npos && !flag ) //Se não tiver quebra de linha no token e não for label, insere o token e a quebra de linha
			{
				temp5 << tokens[i] << ' ';
			}
			else if((tokens[i].find('\n') != string::npos) && flag && tokens[subcounter].find("IF") == string::npos) //Se a diretiva de IF for falsa, reseta a flag apenas no final da próxima linha
			{	
				flag = false;
			}
			else if((tokens[i].find("IF") != string::npos) && flag)
			{
				i++;
				flag = false;
				continue;
			}
			else if((tokens[i].find("IF") != string::npos) && !flag && (tokens[i+1].find("1") != string::npos))
			{
				i++;
				continue;
			}
		}

		/*for (tokens_it = tokens.begin(); tokens_it != tokens.end(); ++tokens_it)
			{
				cout << *tokens_it << endl;
			}*/

		tokens.clear();			//Limpa o vector com os tokens
		temp2.str("");			//Limpa o stringstream com as linhas do arquivo
	}



	istringstream is3(temp5.str());		//Passagem para o arquivo intermediário
	while(getline(is3,words3))
	{
		if(words3.find("EQU") == string::npos && words3 != "")	//Não passa linhas com diretiva EQU ou linhas em branco
			no_comments << words3 << endl;
	}

	/*for(equ_list_it = equ_list.begin();	equ_list_it != equ_list.end(); equ_list_it++)
		cout << *equ_list_it; //PRINTAR A LISTA COM OS EQU */

	return srce_file.eof() && no_comments.good(); //Se deu tudo certo, retorna true
}

int main(int argv,char**argc)		//Apenas ambiente de teste, o importante é a função acima
{
	if(argv!=3){
		cout<<"error: numero de arquivos invalidos";
	}


	ifstream my_file(argc[1]);
	fstream file_out(argc[2], ios::out|ios::in|ios::trunc);
	pre_proc(my_file,file_out);
	my_file.close();
	file_out.close();
	return 0;
}
