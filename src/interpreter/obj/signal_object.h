#ifndef OBJ_SIGNAL_H
#define OBJ_SIGNAL_H

#include <interpreter/obj/object.h>

class eval_exception;

class context;

/**
 * Represents a 'signal', the major method for raising and handling errors in fl. Signals
 * can be raised in two ways: a) the interpreter evaluates a 'raise' expression, and b)
 * an internal eval_exception is raised. 
 *
 */
class signal_object : public object
{
public:
    signal_object(context*,fclass& = *builtins::signal::get_class());
    virtual void render( std::ostream& os, bool abbrev=true );

    /**
     * Call this to set the node that caused the signal to be raised. This is needed for
     * handlers to be traced back and invoked. 
     */
    void set_source_node( ast* );

    /**
     * Returns a pointer to the node that caused the signal to be raised. 
     * Can be nullptr if the source of the signal is unknown. This should be considered 
     * to be a fatal error, as no handlers can be called. 
     */ 
    ast* source() const { return _source_node; }

    /**
     * Attempts to handle the singal, by tracing back from the node that caused the signal
     * to be raised, to find an appropriate handler. If found, the handler is invoked and
     * the result of its evaluation is returned. 
     * @note If a handler cannot be found, a bare eval_exception is raised, terminating
     * the evaluation of the expression, and no result of the evaluation is evailable.
     */ 
    objref handle(context* pContext,ast* rootNode);

protected:

    ast* _source_node{nullptr};	///< Pointer to the node which raised the signal 
};

/**
 * Represents an eval_exception signal. Allows the program to catch and handle non-fatal
 * evaluation exceptions via the signal mechanism.
 */
class eval_signal_object : public signal_object
{
 public:

    eval_signal_object(context*,
			  eval_exception* pEx=nullptr,
			  fclass& = *builtins::eval_signal::get_class());

    virtual ~eval_signal_object();

    eval_exception* exception() { return _exception; }
    void set_exception( eval_exception* pEx ) { _exception = pEx; }

 protected:

    eval_exception* _exception{nullptr};
};

#endif
