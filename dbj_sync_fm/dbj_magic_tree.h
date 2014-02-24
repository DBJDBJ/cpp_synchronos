#pragma once
namespace dbj {
/*
-----------------------------------------------------------
DBJ		2014 FEB 24 		WIP !
							I think I am trying to do compile time
							tree here ...
							We have 4 key abstractions:
							INodeData 	and data_ptr
							INode		and node_ptr
*/
#define DBJ_MAGIC_TREE_TEST = (1==1)

// hereby after this typedef everything bellow will do only tree of char's
typedef std::auto_ptr<char> data_ptr ;

__interface INodeData { };

// default data is null char: '\x'
// which is a bad design in here ... Data should be abstract root template
// of concrete Data types ... eg Data<char>
class Data : public INodeData {
	data_ptr load_ ;
public :
	Data( data_ptr D_ = data_ptr('\x0') ) : load_(D_) 
	{ 
	#if DBJ_MAGIC_TREE_TEST
		std::cout << "Data [" << (data_ptr::element_type)this->load_.get() << "] created " << std::endl ;
	#endif
	}
	Data( data_ptr::element_type & D_ ) : load_( data_ptr(& D_) ) 
	{ 
	#if DBJ_MAGIC_TREE_TEST
		std::cout << "Data [" << (data_ptr::element_type)this->load_.get() << "] created " << std::endl ;
	#endif
	}
	const data_ptr & load () const { return load_; }
}	;
/*-----------------------------------------------------------*/
__interface INode
{
};
	typedef std::auto_ptr<INode> node_ptr ;
/*-----------------------------------------------------------*/
class NullNode : public INode
{
public:
	typedef std::auto_ptr<NullNode> ptr_type ;
private :
	NullNode () ;
	NullNode (const NullNode & ) ;
	NullNode & operator = (const NullNode & ) ;
};


template< typename L = typename NullNode::ptr_type , 
	typename P =  typename NullNode::ptr_type, 
	typename R =  typename NullNode::ptr_type > 
class Node
{
	public:
		typedef Node * ptr_type ; /* NOTE: this will be the pointer to the instance of a template which is an concrete type */

		Node ( node_ptr d ) : 
			id_(newid_()), data_(d)
		{
	#if DBJ_MAGIC_TREE_TEST
			std::cout << "Node [" << id_ << "] contains : " << data_.id() << std::endl 
				      << "It's parent's id is [" << parent_.id() << "]" << std::endl ;
	#endif
		}
	~Node ()
	{
	}
	const int id () const { return id_ ; }
	operator const ptr_type () const { return this; }
private:
	/* no copying allowed */
	    Node ( Node const & ) ;
		Node const & operator=(Node const &);

	static int counter ;
	int id_   ;
	P parent_ ;
	L left_ ;
	R right_;
	node_ptr data_ ; /// probably  a wrong name
	int newid_ () { return Node::counter += 1 ; }

} ;

template< class L , class P, class R > int Node<L,P,R>::counter = 0 ;

typedef Node<> null_node_type ;

#if DBJ_MAGIC_TREE_TEST

/*-----------------------------------------------------------*/
int dbj_magic_tree_test ( int argc = 0, _TCHAR ** argv = 0 )
{
    // making tree of null's, that is: null chars
	typedef Node<> root_type ;
	typedef Node<root_type, root_type, root_type > tree_type ;

	root_type 		root(Data()) ;
	tree_type   	tree_(Data());

	return 0 ;
}
#endif

} // namespace dbj
