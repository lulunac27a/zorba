/* -*- mode: c++; indent-tabs-mode: nil; tab-width: 2 -*-
 *
 *  $Id: xml_handler.cpp,v 1.1.1.1 2006/10/09 06:58:38 Paul Pedersen Exp $
 *
 *  Copyright 2006-2007 FLWOR Foundation.
 *	Author: John Cowan, Paul Pedersen
 *
 */

#include <iostream>
#include <string>

#include "../context/common.h"
#include "../context/context.h"
#include "../runtime/zorba.h"
#include "../values/values.h"
//daniel #include "../errors/xqp_exception.h"
#include "../util/tokenbuf.h"
#include "../util/tracer.h"
#include "../util/URI.h"
#include "xml_handler.h"

using namespace std;
namespace xqp {

#define DELIMSET " \n,.;:?!\r\t+-_!@#$%&*()[]{}=/|\\<>'\""

xml_handler::xml_handler(
	zorba* _zorba_p,
	string const&  _baseuri,
	string const&  _uri,
	vector<xml_term>& _term_v)
:
	scan_handler(),
	top(0),
	the_attribute(attrname_t(0,"")),
	the_element(""),
	the_PCDATA(""),
	the_PITarget(""),
	the_entity(0),
	term_pos(0),
	last_pos(0),
	uri(URI(_baseuri+'/'+_uri).hashkey()),
	term_v(_term_v),
	zorba_p(_zorba_p),
	istore(*zorba_p->istore())
{
	itemref_t baseuri_ref = istore.eos();
	new(istore) xs_stringValue(istore,_baseuri);

	itemref_t uri_ref = istore.eos();
	new(istore) xs_stringValue(istore,_uri);

	baseref = istore.eos();
	document_node* d_p = new(istore) document_node(zorba_p,baseuri_ref,uri_ref);

	nodeid_t nid = d_p->id();
	zorba_p->put_dnid(uri, nid);
	zorba_p->put_noderef(nid, baseref);
}

/*daniel
void xml_handler::error(string const& msg) const
throw (xqp_exception)
{
	throw xqp_exception("XML_HANDLER", msg);
}
*/

inline void xml_handler::add_term(xml_term const& term)
{
	term_v.push_back(term);
}


// attribute without value callback
void xml_handler::adup(const char* buf, int offset, int length)
{
	if (length==0) return;
	string name(buf,offset,length);
	string prefix(NON_PREFIX);
	string::size_type loc = name.find(':', 0);
	if (loc!=string::npos) {
		prefix = name.substr(0,loc);
		name = name.substr(loc+1);
	}

	// find namespace
	itemref_t uriref = istore.namespace(prefix);
	the_attribute = attrname_t(uriref,name);

	// store for load on start tag close
	qnamepair_t qn = istore.add_qname(uriref,name);
	attr_v.push_back(attrpair_t(qn,""));
}


// attribute name callback
void xml_handler::aname(const char* buf, int offset, int length)
{
	if (length==0) return;
	the_attribute = string(buf,offset,length);
	string::size_type n = the_attribute.find(':');
	if (n!=string::npos) {
		string prefix = the_attribute.substr(0,n);
		nskey_t nskey;
		zorba_p->get_nskey(prefix,nskey);
	}
}


// attribute value callback
void xml_handler::aval(const char* buf, int offset, int length)
{
#ifdef DEBUG
	cout << "@" << the_attribute << "=" << string(buf,offset,length) << endl;
#endif
	if (length==0) return;
	const char* p = &buf[offset];
	tokenbuf tokbuf(p,0,length,DELIMSET);
	tokbuf.set_lowercase();
	tokenbuf::token_iterator it = tokbuf.begin();
	tokenbuf::token_iterator end = tokbuf.end();

	// index terms
	for (; it!=end; ++it) {
		const string& term = *it;
		if (term.length()==0) continue;
		string attr_term = '@'+the_attribute+"/word::"+term;
		add_term(xml_term(attr_term,uri,term_pos++));
		string elem_attr_term = the_element+"/@"+the_attribute+"/word::"+term;
		add_term(xml_term(elem_attr_term,uri,term_pos++));
	}
	
	// store 
	itemref_t attr_ref = istore.eos();
	new(istore) qname_value(istore,the_attribute.URIEF,the_attribute.ATTRNAME);
	attr_v.push_back(attrpair_t(attr_ref,string(buf,offset,length)));
}


// &ent; entity callback
void xml_handler::entity(const char* buf, int offset, int length)
{
#ifdef DEBUG
cout << '&' << string(buf,offset,length) << ';' << endl;
#endif
	string s(buf,offset,length);
	unsigned short code;
  entityMap.get(s, code);
	if (code==0) return;
	ent = (char)code;
}


// serialize concatenated text node
void xml_handler::flush_textbuf_as_text_node()
{
	if (textbuf.str().length()>0) {
		itemref_t ref = istore.eos();
		text_node* t_p = new(istore) text_node(zorba_p, textbuf.str());
		zorba_p->put_noderef(t_p->id(),ref);
		textbuf.str("");
	}
}


// eof callback
void xml_handler::eof(const char* buf, int offset, int length)
{
#ifdef DEBUG
cout << "===== eof =====" << endl;
#endif

	// serialize concatenated text node
	flush_textbuf_as_text_node();

	// set document_node length
	node* n_p =  new(istore,baseref) node();
	n_p->length() = istore.eos() - baseref;
	cout << TRACE << " : set length = " << (istore.eos() - baseref) << endl;
	cout << TRACE << " : baseref = " << baseref << endl;
}


// end tag callback
void xml_handler::etag(const char* buf, int offset, int length)
{
	if (length==0) return;
	qnamekey_t key = hashfun::h64(string(buf,offset,length));// XXX

	// serialize concatenated text node
	flush_textbuf_as_text_node();
	
	// clear stack to matching tag
	stack<elempair_t> recover_stack;
	while (!node_stack.empty()) {
		elempair_t ep = node_stack.top();
		node_stack.pop();
		if (ep.first==key) {
			itemref_t ref = ep.second;
			itemref_t eref = istore.eos();
			node* n_p = new(istore,ref) node();
			n_p->m_length = (eref - ref);
			cout << TRACE << " : set length = " << (eref - ref) << endl;
			cout << TRACE << " : length = " << n_p->m_length << endl;
			break;
		}
		else {
			recover_stack.push(ep);
		}
	}
	while (!recover_stack.empty()) {
		elempair_t ep= recover_stack.top();
		recover_stack.pop();
		node_stack.push(ep);
	}
}


// general identifier = tag callback
void xml_handler::gi(const char* buf, int offset, int length)
{
	if (length==0) return;
	string name(buf,offset,length);
	name_stack[top++] = name;
	itemref_t qname_ref = istore.eos();
	qname_value* q_p = new(istore) qname_value(zorba_p,name);
	itemref_t ref = istore.eos();
	element_node* e_p = new(istore) element_node(zorba_p,qname_ref);
	zorba_p->put_noderef(e_p->id(),ref);
	node_stack.push(elempair_t(q_p->qnamekey(),ref));

	// serialize concatenated text node
	flush_textbuf_as_text_node();
}


// add an index term
void xml_handler::add_term(
	string const& term,
	uint64_t uri,
	uint32_t pos)
{
	string e;			// element node
	string p;			// parent node
	string gp;		// grandparent node
	string ggp;		// great-grandparent

	if (term_indexing) {
		add_term(xml_term(term, uri, pos));
	}
	if (top>=1) {
		e = name_stack[top-1];
		e += "/word::"+term;
		if (e_indexing) add_term(xml_term(e, uri, pos));
	}
	if (top>=2) {
		p = name_stack[top-2];
		p += "/child::"+e;
		if (p_indexing) add_term(xml_term(p, uri, pos));
	}
	if (top>=3) {
		string gp = name_stack[top-3];
		gp += "/child::"+p;
		if (gp_indexing) add_term(xml_term(gp, uri, pos));
	}
	if (top>=4) {
		ggp = name_stack[top-4];
		ggp += "/child::"+gp;
		if (ggp_indexing) add_term(xml_term(ggp, uri, pos));
	}
}


// parsed content (tag body) callback
void xml_handler::pcdata(const char* buf, int offset, int length)
{
#ifdef DEBUG
	cout << "pcata = |"<<string(buf,offset,length)<<"| ["<<length<<"]\n";
#endif

	if (length==0) return;

	// tokenize for search indexing
	tokenbuf tokbuf(buf,offset,length,DELIMSET);
	tokbuf.set_lowercase(true);
	string last("");

	tokenbuf::token_iterator it = tokbuf.begin();
	for (;it!=tokbuf.end(); ++it) {
		string const& term = *it;
#ifdef DEBUG
		cout << "pcdata::term = "<<term<<", pos = "<<it.get_token_pos()<<endl;
#endif
		if (term.length()==0) continue;
		add_term(term, uri, term_pos);
		if (bigram_indexing && last_pos>0) {
			string bigram = last+"#"+term;
			add_term(bigram, uri, last_pos);
		}
		last_pos = term_pos;
		last = term;
		++term_pos;
	}

	// concat text
	textbuf << string(buf,offset,length);
}


// processing instruction callback
void xml_handler::pi(const char* buf, int offset, int length)
{
#ifdef DEBUG
	string s(buf,offset,length);
	cout << "<?" << s << endl;
#endif
}


// processing instruction target callback
void xml_handler::pitarget(const char* buf, int offset, int length)
{
#ifdef DEBUG
	string s(buf,offset,length);
	cout << " " << s << ">" << endl;
#endif
}


// start tag close (attributes all processed) callback 
void xml_handler::stagc(const char* buf, int offset, int length)
{
#ifdef DEBUG
	cout << ">" << endl;
#endif

	// serialize attribute list
	vector<attrpair_t>::const_iterator it = attr_v.begin();
	for (; it!=attr_v.end(); ++it) {
  	attrpair_t p = *it;
		itemref_t qname_ref = p.first;
		string val = p.second;
		itemref_t ref = istore.eos();
		attribute_node* a_p = new(istore) attribute_node(zorba_p,qname_ref,val);
		zorba_p->put_noderef(a_p->id(), ref);
	}
	attr_v.clear();
}


// comment callback
void xml_handler::cmnt(const char* buf, int offset, int length)
{
#ifdef DEBUG
	string s(buf,offset,length);
	cout << "<!--" << s << "-->" << endl;
#endif
}


}	/* namespace xqp */
