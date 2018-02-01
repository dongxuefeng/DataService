#ifndef _cxx_xml_h__20140121__
#define _cxx_xml_h__20140121__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

#include <map>
#include <vector>
using namespace std;

class cxx_xml {
private:
	//prop "/userdata/data:propname","propvalue"
	//text "/userdata/data","textvalue"
	map<string, string> ptag; //path tag
	vector<string> vcttag; //vector
	//load fucation
	string current_tagpath; // /userdata/data/type
	string current_propname;
private:
	int create_current_tag(string new_tag) {
		vcttag.push_back(new_tag);
		/*
		 for(vector<string>::const_iterator iter = vcttag.begin();
		 iter != vcttag.end();
		 iter ++
		 {
		 current_tagpath += "/"+iter->c_str();
		 };
		 */
		current_tagpath = current_tagpath + "/" + new_tag;
		return vcttag.size();
	}
	;
	int pop_tagpath() {
		//cout << current_tagpath << endl;
		//cout << vcttag.size() << endl;

		if(vcttag.size() > 0)
		{
			vcttag.pop_back();
			//pop tag
			current_tagpath = "";
			for (vector<string>::const_iterator iter = vcttag.begin(); iter
					!= vcttag.end(); iter++) {
				current_tagpath.append("/");
				current_tagpath.append(iter->c_str());
			};
		}
		//else
			//cout << current_tagpath << endl;
		return 0;
	}
	;
	void add_current_prop(string new_propvalue) {
		ptag.insert(pair<string, string> (current_propname, new_propvalue));
	}
	;
	void new_propname(string new_prop) {
		current_propname = current_tagpath + ":" + new_prop;
	}
	;
	void create_current_text(string new_textvalue) {
		ptag.insert(pair<string, string> (current_tagpath, new_textvalue));
	}
	;

public:
	void print_xmldata() {
		int ii = 0;
		for (map<string, string>::const_iterator iter = ptag.begin(); iter
				!= ptag.end(); iter++, ii++) {
			//cout << ii << ":" << iter->first << " = " << iter->second << endl;
			printf("%d:%s = %s\n", ii, iter->first.c_str(), iter->second.c_str());
		};
	}
public:
	bool xpath_getprop(const char * tagpath, const char * propname,
			string & propvalue) {
		bool bresult = false;
		string strtmp = tagpath;
		strtmp.append(":");
		strtmp.append(propname);
		map<string, string>::const_iterator iter = ptag.find(strtmp);
		if (iter != ptag.end()) {
			propvalue = iter->second;
			bresult = true;
		} else {
			propvalue = "";
		}
		return bresult;
	}

	const char * xpath_getprop(const char * tagpath, const char * propname)
	{
		string propvalue = "";
		xpath_getprop(tagpath,propname,propvalue);
		return propvalue.c_str();
	}

	bool xpath_gettext(const char * tagpath, string & ptestvalue) {
		bool bresult = false;

		map<string, string>::const_iterator iter = ptag.find(tagpath);
		if (iter != ptag.end()) {
			ptestvalue = iter->second;
			bresult = true;
		} else {
			ptestvalue = "";
		}
		return bresult;
	}
public:
	void readxml(const char * xmldata) {
		int current_station = 0;
		string tmp_str = "";
		vcttag.clear();
		int bspace = 0;
		char * p = (char *) xmldata;
		for (; *p != 0; p++)
		{

			//解决引号配对问题
			if((*p != '"') && (*p != '\''))
			if(bspace)
			{
				if(current_station == 21 )
				{
					tmp_str.append(1, *p);
					continue;
				}
			}

			if(current_station == 30)
			{
				if(*p != '<' )
				{
					tmp_str.append(1, *p);
				    continue;
				};
			};



			switch (*p) {
			case '<':
				//add tag text
				if (current_station == 30)
					if (tmp_str.length()) {
						create_current_text(tmp_str);
						tmp_str = "";
					};

				current_station = 10; //1.1set station tag
				tmp_str = "";
				break;
			case '>':
				if (current_station == 10) {
					if (tmp_str.length()) {
						//add tag
						create_current_tag(tmp_str);
						tmp_str = "";
					};

				};
				//after text name
				if (current_station == 21) {
					add_current_prop(tmp_str);
					tmp_str = "";
				}
				;
				//this is two station
				if (*(p - 1) == '/') {
					//this is tagpath
					pop_tagpath();
					current_station = 0;
				} else {
					//this is tag text
					current_station = 30;
					tmp_str = "";
				}
				break;
			case '"':
			case '\'':
				if(current_station == 21 || current_station == 30)
					bspace = !bspace;
				break;
			case '/':
				if (current_station == 21) {
					add_current_prop(tmp_str);
					tmp_str = "";
					//2.1after prop name
					current_station = 20;
				}
				;
				//this is two station
				if (*(p - 1) == '<') {
					//this is tag end
					pop_tagpath();
					current_station = 0;
				}
				;

				break;
			case '?':
				current_station = 0;
				break;
			case ' ':
				if (current_station == 10)
					if (tmp_str.length()) {
						//1.2 tag finish
						create_current_tag(tmp_str);
						tmp_str = "";
						//2.1after prop name
						current_station = 20;
					}
				;
				if (current_station == 21)
					//if (tmp_str.length())   // prop value is "",
					{
						add_current_prop(tmp_str);
						tmp_str = "";
						//2.1after prop name
						current_station = 20;
					}
				;

				break;
			case '=':
				if (current_station == 20) {
					if (tmp_str.length()) {
						//add propname
						new_propname(tmp_str);

						//after prop value
						current_station = 21;
						tmp_str = "";
					};
				}
				;

				break;
			default:
				if (current_station != 0) {
					tmp_str.append(1, *p);
				}
				break;

			}

		};
	}
	;
};

/* test

 int main()
 {
 string requestxml  = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
 requestxml = requestxml + "<userdata>"
 +"<head>"
 +"<type value=\"0001\"/>"  //	��������
 +"<user value=/>"
 +"<dev value=/>"
 +"<Request_Index value=\"1\"/>"//����ʼ����ţ������Ҫ��!�������ʱ��Ч
 +"</head>"
 +"<data>"
 +"<type value=\"0001\"/>"   //ҵ������
 +"<platenum value=\"陕12345\"/>"
 +"<platetype value=\"01\"/>	"
 +"<drivinglicense value=\"111111111111111\"/>"
 +"</data>"
 +"</userdata>";
 cout << requestxml << endl;
 cxx_xml myxml;
 myxml.readxml(requestxml.c_str());
 myxml.print_xmldata();
 int i;
 cin >> i;
 return 0;
 }

 * */

#endif //_cxx_xml_h__20140121__
