#include <cstdio>
#include <string>
#include <cstdlib>
#include <vector>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <unordered_map>

#include <iostream>

#include <lttoolbox/xml_parse_util.h>

#include "parse_ref.h"

void print_tags(vector< wstring > input)
{
	for (int i = 0; i < input.size(); i++) 
	{
		wcout << input[i];
		cout << " ";
	}
}

//  g++ parse_ref.cc -I /opt/local/include/libxml2/ -L /usr/lib -lxml2 -lz -lpthread -lm
vector<wstring> ParseRef::parseTags (wstring tags)
{
	vector<wstring> temp_tags_list;

	wstring temptag;

	for (std::wstring::iterator i = tags.begin(); i != tags.end(); ++i)
	{
		if(*i == '\\') //dealing with escaped characters
		{
			temptag.push_back(*i);
			++i;
			temptag.push_back(*i);
		}
		else if(*i == '.')
		{
			temp_tags_list.push_back(temptag);
			temptag.clear();
		}
		else
		{
			temptag.push_back(*i);
		}
	}

	if(!temptag.empty()) //if any tag remaining
		temp_tags_list.push_back(temptag);

	print_tags(temp_tags_list);

	cout << "\n";

	return temp_tags_list;
}

void ParseRef::parseParameterItem (xmlDocPtr doc, xmlNodePtr cur, wstring parameter_name) 
{
	xmlChar *Attr;
	cur = cur->xmlChildrenNode;

	vector <wstring> temp_tags_list;

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"parameter-item"))) 
	    {
	    	Attr = xmlGetProp(cur, (const xmlChar *)"tags");

	    	printf("ParameterItem: ");

	    	temp_tags_list = parseTags(XMLParseUtil::towstring(Attr));
	    	parameters[parameter_name].push_back(temp_tags_list);

	    	temp_tags_list.clear();

		    xmlFree(Attr);
		    
		    //key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
 	    }

		cur = cur->next;
	}
    return;
}

void ParseRef::parseParameters (xmlDocPtr doc, xmlNodePtr cur) 
{
	wstring parameter_name;

	cur = cur->xmlChildrenNode;

	while (cur != NULL) 
	{
		if(cur->type == XML_ELEMENT_NODE)
		{
			parameter_name = XMLParseUtil::towstring(cur->name);

			cout << "\n";
	    	wcout << parameter_name;
	    	cout << "\n";
	    	
	    	parseParameterItem(doc,cur,parameter_name);
	    }
	    	
		cur = cur->next;
	}
    return;
}

void ParseRef::parseCatItem (xmlDocPtr doc, xmlNodePtr cur, wstring cat_name) 
{
	xmlChar *Attr;
	cur = cur->xmlChildrenNode;

	vector <wstring> temp_tags_list;

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"cat-item"))) 
	    {
	    	Attr = xmlGetProp(cur, (const xmlChar *)"tags");
	    	printf("catItem: ");

	    	temp_tags_list = parseTags(XMLParseUtil::towstring(Attr));
		    cats[cat_name].push_back(temp_tags_list);

		    temp_tags_list.clear();

		    xmlFree(Attr);
		    
		    //key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
 	    }

		cur = cur->next;
	}
    return;
}

void ParseRef::parseCats (xmlDocPtr doc, xmlNodePtr cur) 
{
	xmlChar *Attr;
	cur = cur->xmlChildrenNode;

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"def-cat")))
	    {
	    	Attr = xmlGetProp(cur, (const xmlChar *)"n");
	    	printf("catName: %s\n", Attr);  
	    	
	    	parseCatItem(doc,cur, XMLParseUtil::towstring(Attr));
	    	xmlFree(Attr);	
	    } 
	    	
		cur = cur->next;
	}
    return;
}

vector<markable_pattern> ParseRef::parsePatternItem (xmlDocPtr doc, xmlNodePtr cur) 
{
	xmlChar *Attr;
	cur = cur->xmlChildrenNode;

	vector<markable_pattern> temp_pattern;

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"pattern-item"))) 
	    {
	    	markable_pattern temp;

	    	Attr = xmlGetProp(cur, (const xmlChar *)"n");
	    	temp.name = XMLParseUtil::towstring(Attr);

	    	wcout << temp.name;
	    	cout << " ";

		    xmlFree(Attr);

		    Attr = xmlGetProp(cur, (const xmlChar *)"head");

		    if(Attr != NULL)
		    {
		    	temp.head = 1;
		    	printf("[HEAD!]");
		    }
		    else
		    	temp.head = 0;

		    xmlFree(Attr);

		    temp_pattern.push_back(temp);
 	    }

		cur = cur->next;
	}

    return temp_pattern;
}

void ParseRef::parsePatterns (xmlDocPtr doc, xmlNodePtr cur, wstring markable_name) 
{
	cur = cur->xmlChildrenNode;

	vector<markable_pattern> temp_pattern_list;

	wcout << markable_name;
	cout << "\n";

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"pattern")))
	    	temp_pattern_list = parsePatternItem(doc,cur);
	    	
	    markables[markable_name].push_back(temp_pattern_list);
	    temp_pattern_list.clear();

		cur = cur->next;

		cout << "\n";
	}
    return;
}

void ParseRef::parseMarkables (xmlDocPtr doc, xmlNodePtr cur) 
{
	xmlChar *Attr;
	cur = cur->xmlChildrenNode;

	while (cur != NULL) 
	{
	    if ((!xmlStrcmp(cur->name, (const xmlChar *)"markable")))
	    {
	    	Attr = xmlGetProp(cur, (const xmlChar *)"n");
	    	printf("MarkableName: ");	
	    	
	    	parsePatterns(doc,cur, XMLParseUtil::towstring(Attr));

	    	xmlFree(Attr);
	    } 
	    	
		cur = cur->next;
	}
    return;
}

void ParseRef::parseDoc(char *docname) 
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);
	
	if (doc == NULL ) 
	{
		fprintf(stderr,"Document not parsed successfully. \n");
		return;
	}
	
	cur = xmlDocGetRootElement(doc);
	
	if (cur == NULL) 
	{
		fprintf(stderr,"Empty Document!\n");
		xmlFreeDoc(doc);
		return;
	}
	
	if (xmlStrcmp(cur->name, (const xmlChar *) "ref")) 
	{
		fprintf(stderr,"Document of the wrong type! Root node should be ref.\n");
		xmlFreeDoc(doc);
		return;
	}
	
	cur = cur->xmlChildrenNode;
	while (cur != NULL) 
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"section-parameters")))
		{
			parseParameters (doc, cur);
		}

		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"section-def-cats")))
		{
			parseCats (doc, cur);
		}

		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"section-markables")))
		{
			parseMarkables (doc, cur);
		}
		 
		cur = cur->next;
	}
	
	xmlFreeDoc(doc);
	return;
}

/* //Code for Testing
int main(int argc, char **argv) 
{
	char *docname;
		
	if (argc <= 1) 
	{
		printf("Usage: %s docname\n", argv[0]);
		return(0);
	}

	docname = argv[1];

	ParseRef ref;

	ref.parseDoc(docname);

	return (1);
}
*/