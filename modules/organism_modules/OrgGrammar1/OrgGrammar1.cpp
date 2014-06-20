using namespace std;
#include <iostream>
#include <algorithm>
#include "OrgGrammar1.h"


void OrgGrammar1::printDevelopmentProcess(ofstream &stream)
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


// Create a new developmentProcess, for an `orphan' cell
void OrgGrammar1::initializeOrphanDevelopmentProcess(config &args)
{
  // Start with everything rewriting to nothing
  rules.resize(ruleAlphabet);
}


// Done at the start of each reproduction cycle
void OrgGrammar1::initializeDevelopmentProcess(config &args)
{
  OrgGrammar1 *c = ((OrgGrammar1*)child);

  // Copy child's genome to decoded genome
  copy(child->genome.begin(), child->genome.end(), back_inserter(c->decodedGenome));

  prevIt = c->decodedGenome.begin();
  prevChanged = false;
}


// When we've finished development the genome, we've have a list `decodedGenome'
// Interpret that genome as the specification of a list of rewrite rules.
// Give that list of rewrite rules to the child. Give it the genome after
// the specification of the rule table as its `body specification'
void OrgGrammar1::interpret(config &args, OrgGrammar1 *c)
{
  // TODO what to print when debugging?
  if (args.debug)
  {
    cout << "Development complete" << endl;
    if (decodedGenome==c->decodedGenome)
      cout << "Parent and child have same development function" << endl;
    cout << endl;
  }

  c->rules.resize(ruleAlphabet);
  list<unsigned short>::iterator bodyStart = c->decodedGenome.end();

  vector<unsigned short> ruleStack;
  for (list<unsigned short>::iterator s = c->decodedGenome.begin() ; s != c->decodedGenome.end(); ++s)
  {
    if (*s >= ruleAlphabet and *s < ruleAlphabet*2) // A' type symbol
    {
      bodyStart = c->decodedGenome.end();
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

  // Copy the remainder of the decodedGenome to the child body
  //copy(bodyStart, c->decodedGenome.end(), back_inserter(c->somaSpecification));
  for(list<unsigned short>::iterator it=bodyStart; it!=c->decodedGenome.end(); ++it)
  {
    if (*it > ruleAlphabet*2)
      c->somaSpecification.push_back(*it);
  }

}


// Iteratively apply the rewrite rules (developmentProcess) to the genome
void OrgGrammar1::updateDevelopment(config &args)
{
  OrgGrammar1 *c = (OrgGrammar1*)child;

  bool changed = false;

  if (prevIt == c->decodedGenome.end())
  {
    prevIt = c->decodedGenome.begin();
    prevChanged = false;
  }

  // Read 10 symbols of the decoded genome, starting where we ended last time
  // If we see any "A" type symbols, apply the relevant rewrite rule
  list<unsigned short>::iterator it;
  int count = 0;
  for (it = prevIt ; it != c->decodedGenome.end(); ++it,++count)
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
	c->decodedGenome.insert(it, rule->begin(), rule->end());
      // Delete the symbol
      it = prev(c->decodedGenome.erase(it));
    }
  }

  // Record where we were up to last time decode was called
  prevIt = it;

  // TODO what to print when debugging?
  if (args.debug)
  {
    cout << endl << "Development step" << endl;
    cout << "\tid: " << id << "  child id: " << child->id << endl;
    for (list<unsigned short>::iterator it = c->decodedGenome.begin() ; it != c->decodedGenome.end(); ++it)
      cout << *it << ".";
    cout << endl << endl;
  }

  // If no changes occured on the latest full pass through the decoded genome,
  // interpret decodedGenome as the specification of a list of rewrite rules.
  // The part of the genome after the specification of the rewrite rules is the
  // `body specification'. Give the new rewrite rules and body spec to the child
  // and go to state reproduced, passing control back to the BaseOrganism
  if (not changed and not prevChanged)
  {
    interpret(args, c);
    state = REPRODUCED;
  }
}


OrgGrammar1::OrgGrammar1(config &args) : BaseOrganism(args)
{
  ruleAlphabet = alphabetSize/3;
}

OrgGrammar1::~OrgGrammar1()
{
}
