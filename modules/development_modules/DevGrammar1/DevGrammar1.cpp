using namespace std;
#include <iostream>
#include <algorithm>
#include "DevGrammar1.h"


void DevGrammar1::printDecoder(ofstream &stream)
{
  int rule = 0;
  stream << "Rewrite rules:" << endl;
  for (vector <vector <unsigned short> >::iterator it = rules.begin() ; it != rules.end(); ++it,++rule)
  {
    stream << rule << " -> ";
    for (vector <unsigned short>::iterator r = it->begin(); r!=it->end(); ++r)
      stream << *r << ",";
    stream << endl;
  }
}


// Create a new decoder, for an `orphan' cell
void DevGrammar1::initializeOrphanDecoder(config &args)
{
  // Start with everything rewriting to nothing
  rules.resize(ruleAlphabet);
}


// Done at the start of each reproduction cycle
void DevGrammar1::initializeDecoding(config &args)
{
  DevGrammar1 *c = ((DevGrammar1*)child);

  // Copy child's data to decoded data
  copy(child->data.begin(), child->data.end(), back_inserter(c->decodedData));

  prevIt = c->decodedData.begin();
  prevChanged = false;
}


// When we've finished decoding the data, we've have a list `decodedData'
// Interpret that data as the specification of a list of rewrite rules.
// Give that list of rewrite rules to the child. Give it the data after
// the specification of the rule table as its `body specification'
void DevGrammar1::interpret(config &args, DevGrammar1 *c)
{
  // TODO what to print when debugging?
  if (args.debug)
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

  // Copy the remainder of the decodedData to the child body
  //copy(bodyStart, c->decodedData.end(), back_inserter(c->bodySpecification));
  for(list<unsigned short>::iterator it=bodyStart; it!=c->decodedData.end(); ++it)
  {
    if (*it > ruleAlphabet*2)
      c->bodySpecification.push_back(*it);
  }

}


// Iteratively apply the rewrite rules (decoder) to the data
void DevGrammar1::decode(config &args)
{
  DevGrammar1 *c = (DevGrammar1*)child;

  bool changed = false;

  if (prevIt == c->decodedData.end())
  {
    prevIt = c->decodedData.begin();
    prevChanged = false;
  }

  // Read 10 symbols of the decoded data, starting where we ended last time
  // If we see any "A" type symbols, apply the relevant rewrite rule
  list<unsigned short>::iterator it;
  int count = 0;
  for (it = prevIt ; it != c->decodedData.end(); ++it,++count)
  {
    if(count>10)
    {
      changed = true;
      break;
    }

    // If we see a "A" type symbol
    if (*it < ruleAlphabet)
    {
      changed = true;
      prevChanged = true;
      vector<unsigned short>  *rule = &(rules.at(*it));
      // Insert what we're rewriting to
      if (rule->size() > 0) 
	c->decodedData.insert(it, rule->begin(), rule->end());
      // Delete the symbol
      it = prev(c->decodedData.erase(it));
    }
  }

  // Record where we were up to last time decode was called
  prevIt = it;

  // TODO what to print when debugging?
  if (args.debug)
  {
    cout << endl << "Decoding step" << endl;
    cout << "\tid: " << id << "  child id: " << child->id << endl;
    for (list<unsigned short>::iterator it = c->decodedData.begin() ; it != c->decodedData.end(); ++it)
      cout << *it << ".";
    cout << endl << endl;
  }

  // If no changes occured on the latest full pass through the decoded data,
  // interpret decodedData as the specification of a list of rewrite rules.
  // The part of the data after the specification of the rewrite rules is the
  // `body specification'. Give the new rewrite rules and body spec to the child
  // and go to state reproduced, passing control back to the BaseDevMechanism
  if (not changed and not prevChanged)
  {
    interpret(args, c);
    state = REPRODUCED;
  }
}


DevGrammar1::DevGrammar1(config &args) : BaseDevMechanism(args)
{
  ruleAlphabet = alphabetSize/3;
}

DevGrammar1::~DevGrammar1()
{
}
