#include "pySRUP.h"

uint8_t get_srup_library_version()
{
    return SRUP::SRUP_VERSION;
}

uint8_t get_srup_library_generic_message_type()
{
    return SRUP::SRUP_MESSAGE_TYPE_GENERIC;
}

uint8_t get_srup_library_initiate_message_type()
{
    return SRUP::SRUP_MESSAGE_TYPE_INITIATE;
}

uint8_t get_srup_library_response_message_type()
{
    return SRUP::SRUP_MESSAGE_TYPE_RESPONSE;
}

uint8_t get_srup_library_activate_message_type()
{
    return SRUP::SRUP_MESSAGE_TYPE_ACTIVATE;
}

uint8_t get_srup_library_action_message_type()
{
    return SRUP::SRUP_MESSAGE_TYPE_ACTION;
}

// Note that since .token() can return a nullptr for an uninitialized object - we need a more complex getter function for the Python 
// property to ensure that we return a None in those cases. We could return "" : but None is more correct...
boost::python::object get_token(SRUP_MSG& self)
{
    const uint8_t* rv;
    rv = self.token();
    if (rv!=nullptr)
        return boost::python::object(std::string((char*)rv, self.token_length()));
    else
        return boost::python::object();
}

void set_token(SRUP_MSG& self, std::string s)
{
    self.token((uint8_t*) s.c_str(), (uint16_t)s.length());
}

// .version() can never be a nullptr – so we don't need the more complex type here...
uint8_t get_version(SRUP_MSG& self)
{
    return *self.version();
}

// Similarly .msgtype() can never be a nullptr – so we don't need the more complex type here...
uint8_t get_msgtype(SRUP_MSG& self)
{
    return *self.msgtype();
}


// As per .token() we can get a nullptr here - so we need to deal with it...
boost::python::object get_sequenceID(SRUP_MSG& self)
{
    const uint64_t* rv;
    rv = self.sequenceID();
    
    if (rv != nullptr)
        return boost::python::object(*rv);
    else
        return boost::python::object();
}

void set_sequenceID(SRUP_MSG& self, uint64_t seqid)
{
    self.sequenceID(&seqid);
}

// ...ditto for the senderID()...
boost::python::object get_senderID(SRUP_MSG& self)
{
    const uint64_t* rv;
    rv = self.senderID();
    
    if (rv != nullptr)
        return boost::python::object(*rv);
    else
        return boost::python::object();
}


void set_senderID(SRUP_MSG& self, uint64_t sender)
{
    self.senderID(&sender);
}

// We shouldn't need to return the signature directly to Python...
/*
boost::python::object get_signature(SRUP_MSG& self)
{
    const uint8_t* rv;
    rv = self.signature();
    if (rv!=nullptr)
        return boost::python::object(std::string((char*)rv));
    else
        return boost::python::object();
}
*/

PyObject* serializer(SRUP_MSG& self)
{
    PyObject *pymemview;
    uint8_t* serialized_data;
    uint16_t serial_size;

    // self.Serialized() will return nullptr if serialization cannot be carried out...
    serialized_data = self.Serialized();
    if (serialized_data != nullptr)
    {
        serial_size = self.SerializedLength();

        pymemview = PyMemoryView_FromMemory((char*) serialized_data, serial_size, PyBUF_READ);
        return PyBytes_FromObject(pymemview);
    }
    else
        return Py_None;

}

bool deserializer(SRUP_MSG& self, boost::python::object py_buffer)
{
    boost::python::stl_input_iterator<uint8_t> begin(py_buffer), end;

    // Copy the py_buffer into a local buffer with known contiguous memory.
    std::vector<uint8_t> buffer(begin, end);

    // Cast and delegate to the printBuffer member function.
    if (self.DeSerialize(reinterpret_cast<uint8_t*>(&buffer[0])))
        return true;
    else
        return false;
}