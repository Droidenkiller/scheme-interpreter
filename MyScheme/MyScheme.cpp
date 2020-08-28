// MyScheme.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#define DEBUG
#define ADD_FACTORIAL

#include <iostream>
#include "Reader.h"
#include "ScmObjectIncludes.h"
#include <string>
#include <stdio.h>
#include "SelfTest.h";
#include "Environment.h"
#include "BuiltInFunctionsTramp.h"

using namespace std;

Environment globalEnvironment;

void initGlobalEnv()
{
	globalEnvironment = Environment();

	globalEnvironment.addSymbol(new ScmObject_Symbol("+"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_ADD>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("-"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_SUBTRACT>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("*"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_MULTIPLY>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("/"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DIVIDE>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("eq?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_EQUALS>());
	globalEnvironment.addSymbol(new ScmObject_Symbol(">"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_GT>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("<"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LT>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("define"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_DEFINE>());
	globalEnvironment.addSymbol(new ScmObject_Symbol("if"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_IF>());
}

int main()
{
	initGlobalEnv();

	Reader reader = Reader();

	if (testScheme(reader))
	{
		cout << "Selftest concluded without errors.\n";
	}
	else
	{
		cout << "Selftest concluded with errors.\nSee above messages for details.\n";
	}

	std::string currentInput;

#ifdef ADD_FACTORIAL
	currentInput = "(define ! (lambda (n) (if (eq? n 1) 1 (* n (! (- n 1))))))";

	ScmObject* obj = reader.ReadNextSymbol(currentInput);

	exec(static_cast<ScmObject_FunctionCall*>(obj)->createFunctionExecution(&globalEnvironment, nullptr));

	delete obj;
#endif

	bool needsMoreData = false;

	while (true)
	{
		getline(cin, currentInput);
		needsMoreData = false;

		while (currentInput.length() > 0 && !needsMoreData)
		{
			ScmObject* object = reader.ReadNextSymbol(currentInput);

			// Later on this means we have not read to the end and need another line
			if (object != nullptr)
			{
				if (object->getType() == ScmObjectType::INTERNAL_ERROR)
				{
					currentInput = "";
					cout << object->getOutputString() << endl;
				}
				else if (object->getType() == ScmObjectType::FUNCTION_CALL)
				{
					object = exec(static_cast<ScmObject_FunctionCall*>(object)->createFunctionExecution(&globalEnvironment, nullptr));
					cout << object->getOutputString() << std::endl;
				}
				else if (object->getType() == ScmObjectType::SYMBOL)
				{
					const ScmObject* obj = globalEnvironment.getSymbol(static_cast<ScmObject_Symbol*>(object));

					if (obj == nullptr)
					{
						cout << "Symbol " << object->getOutputString() << " is not defined." << std::endl;
					}
					else
					{
						cout << obj->getOutputString() << std::endl;
					}
				}
				else
				{
					cout << object->getOutputString() << endl;
				}
			}
			else
			{
				needsMoreData = true;
			}

			delete object;
		}
	}
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
