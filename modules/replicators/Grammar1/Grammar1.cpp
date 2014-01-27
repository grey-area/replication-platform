using namespace std;
#include <iostream>
#include <algorithm>
#include "Grammar1.h"

// Create a new decoder, for an `orphan' cell
void Grammar1::newDecoder()
{
  rules.resize(ruleAlphabet);
  /*rules[0].push_back(1);
  rules[1].push_back(6);
  rules[1].push_back(6);
  rules[1].push_back(6);
  rules[1].push_back(6);
  rules[1].push_back(6);
  rules[1].push_back(6);*/
  /*int i;
  for (i=0; i<alphabetSize; i++)
  rules[i] = vector<unsigned short> (1, i);*/
}

// Done at the start of every reproduction cycle
void Grammar1::initializeDecoding(int debug)
{
  // Copy child's data to decoded data
  copy(child->data.begin(), child->data.end(), back_inserter(((Grammar1*)child)->decodedData));

  if (debug)
  {
    cout << endl << "Initializing decoding" << endl;
    cout << "\tid: " << id << "  child id: " << child->id << endl;
    cout << "My data" << endl;
    printData(data);
    cout << "Child data" << endl;
    printData(child->data);
  }
}

void Grammar1::decode(int debug)
{
  Grammar1 *c = (Grammar1*)child;

  bool changed = false;

  // do one (truncated) pass through decodedData, replacing anything that occurs in the rewrite rules.
  for (list<unsigned short>::iterator it = c->decodedData.begin() ; it != c->decodedData.end(); ++it)
  {
    if (*it < ruleAlphabet)
    {
      changed = true;
      vector<unsigned short>  *rule = &(rules.at(*it));
      if (rule->size() > 0) // if there is a rewrite rule
	c->decodedData.insert(it, rule->begin(), rule->end());
      it = prev(c->decodedData.erase(it));
    }

    // TODO temp fix
    if (c->decodedData.size() > maxDataSize*3)
    {
      changed = false;
      c->decodedData.clear();
      break;
    }
  }

  if (debug)
  {
    cout << endl << "Decoding step" << endl;
    cout << "\tid: " << id << "  child id: " << child->id << endl;
    for (list<unsigned short>::iterator it = c->decodedData.begin() ; it != c->decodedData.end(); ++it)
      cout << *it << ".";
    cout << endl << endl;
  }

  // if no changes occured, interpret decodedData as a rule table and some other stuff. Pass the rule table to the child

  if (not changed)
  {

    if (debug)
    {
      cout << "Decoding complete" << endl;
      if (decodedData==c->decodedData)
	cout << "Parent and child have same decoding function" << endl;
      cout << endl;
    }

    c->rules.resize(ruleAlphabet);
    list<unsigned short>::iterator bodyStart = c->decodedData.end();

    vector<unsigned short> ruleStack;
    for (list<unsigned short>::iterator s = c->decodedData.begin() ; s != c->decodedData.end(); ++s)
    {
      if (*s >= ruleAlphabet and *s < ruleAlphabet*2) // A' type symbol
      {
	bodyStart = c->decodedData.end();
	unsigned short rewriteS = *s - (unsigned short)ruleAlphabet;
	// If we're not inside a rule for this symbol already
	if (find(ruleStack.begin(), ruleStack.end(), rewriteS) == ruleStack.end())
	{
	  if (ruleStack.size()>0) // if we were already working on something
	  {
	    (c->rules[ruleStack.back()]).push_back(rewriteS); // add A to that rule
	  }
	  if (c->rules[rewriteS].size() == 0) // if we haven't already worked on this rule
	  {
	    ruleStack.push_back(rewriteS);
	    c->rules[rewriteS].push_back(*s);
	  }
        }
      } // End of A' type symbol
      if (*s >= ruleAlphabet*2) // a type symbol
      {
	if (ruleStack.size()>0) // if we're building a rule
	{
	  c->rules[ruleStack.back()].push_back(*s);
	  if (*s == ruleAlphabet*3) // terminate symbol
	  {
	    ruleStack.pop_back();
	    bodyStart = next(s);
	  }
	}
      } // End of a type symbol
    } // End of interpretation loop

    /*cout << "Child's rule table" << endl;
    for(int i = 0; i<ruleAlphabet; i++)
    {
      cout << i << " -> ";
      for(vector<unsigned short>:: iterator it = c->rules[i].begin(); it != c->rules[i].end(); ++it)
	cout << *it << ".";
      cout << endl;
      }*/

    // Copy the remainder of the decodedData to the child body
    copy(bodyStart, c->decodedData.end(), back_inserter(c->bodySpecification));

    state = REPRODUCED;
    //cout << "REPROCUDED" << endl;
  }
}

Grammar1::Grammar1()
{
  ruleAlphabet = alphabetSize/3;
}

Grammar1::~Grammar1()
{
}
