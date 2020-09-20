// MyScheme.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#define DEBUG
#define ADD_FACTORIAL
#define RUN_SELFTEST

#include <iostream>
#include "Reader.h"
#include "ScmObjectIncludes.h"
#include <string>
#include <stdio.h>
#include "SelfTest.h";
#include "Environment.h"
#include "BuiltInFunctionsTramp.h"
#include <memory>

using namespace std;

shared_ptr<Environment> globalEnvironment;

void initGlobalEnv()
{
	globalEnvironment = make_shared<Environment>();

	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("+"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_ADD>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("-"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_SUBTRACT>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("*"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_MULTIPLY>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("/"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DIVIDE>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("cons"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CONS>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("car"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CAR>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("cdr"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CDR>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("eq?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_EQUALS>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>(">"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_GT>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("<"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LT>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("string?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_STRINGQ>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("number?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_NUMBERQ>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("cons?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_CONSQ>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("function?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTIONQ>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("udf?"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_USER_DEFINED_FUNCTIONQ>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("print"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_PRINT>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("display"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_DISPLAY>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("function-body"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTION_BODY>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("function-arglist"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_FUNCTION_ARG_LIST>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("read-line"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_READ_LINE>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("load"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::BUILT_IN_LOAD>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("quote"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_QUOTE>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("lambda"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_LAMBDA>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("define"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_DEFINE>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("if"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_IF>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("set!"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_SET>());
	globalEnvironment->addSymbol(make_shared<ScmObject_Symbol>("begin"), ScmObject_FunctionDefinition::getFunctionDefinition<ScmObject_FunctionDefinition::FunctionType::SYNTAX_BEGIN>());
}

int main()
{
	initGlobalEnv();

#ifdef RUN_SELFTEST
	if (testScheme())
	{
		cout << "Selftest concluded without errors.\n\n";
	}
	else
	{
		cout << "Selftest concluded with errors.\nSee above messages for details.\n\n";
	}

	// Reinitialize the global environment to remove possible changes from the selftest.
	initGlobalEnv();
	cout << "Reinitialized environment to remove symbols from selftest..." << endl;
#endif

	std::string currentInput;

#ifdef ADD_FACTORIAL
	currentInput = "(define ! (lambda (n) (if (eq? n 1) 1 (* n (! (- n 1))))))";

	std::shared_ptr<ScmObject> obj = Reader::ReadNextSymbol(currentInput);

	exec(static_pointer_cast<ScmObject_FunctionCall>(obj)->createFunctionExecution(nullptr, nullptr));

	cout << "Defined ! (factorial function). Remove ADD_FACTORIAL symbol in code to not add this function." << endl;
#endif

	bool needsMoreData = false;

	while (true)
	{
		getline(cin, currentInput);
		needsMoreData = false;

		while (currentInput.length() > 0 && !needsMoreData)
		{
			std::shared_ptr<ScmObject> object = Reader::ReadNextSymbol(currentInput);

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
					object = exec(static_pointer_cast<const ScmObject_FunctionCall>(object)->createFunctionExecution(nullptr, nullptr));
					if (object != nullptr)
						cout << object->getOutputString() << std::endl;
				}
				else if (object->getType() == ScmObjectType::SYMBOL)
				{
					shared_ptr<const ScmObject> obj = globalEnvironment->getSymbol(static_pointer_cast<ScmObject_Symbol>(object));

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
