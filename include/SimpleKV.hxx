#if !defined(SIMPLEKV_HXX__)
#define SIMPLEKV_HXX__ (1)

#if !defined(WIN32)
#include <stdio.h> /* NULL */
#endif

template<class TK, class TV>
class SimpleKVMaker;

template<class TK, class TV>
class kv_t {
public: TK key;
public: TV value;
};

template<class TK, class TV>
class kvo_t {

public: bool set;
public: TK key;
public: TV value;
};


/*
 * NOT thread safe !! => handle out the class
 */
template<class TK, class TV>
class SimpleKV
{

friend class SimpleKVMaker<TK, TV>;



protected: typedef bool (equ_t)(TK key1, TK key2);

protected: class KV {
public: KV() {
	this->next = NULL;
}

public: kv_t<TK, TV> kv;
public: KV * next;
};

#if !defined(WIN32)
protected: SimpleKV (SimpleKV::equ_t * equ) {
#else
protected: SimpleKV (equ_t * equ) {
#endif
	this->equ = equ;
	this->kvsHead = NULL;
	this->kvsTail = NULL;
	this->count = 0;
}


public: ~SimpleKV () {
	this->empty();
}


public: TK key(TV value);
public: TV value(TK key);


public: bool append (TK key, TV value)
{

	bool e = this->hasKey(key);

	if (e) {
		return false;
	}

	KV * kv = new KV();
	kv->kv.key = key;
	kv->kv.value = value;

	if (NULL != this->kvsTail) {
		this->kvsTail->next = kv;
		this->kvsTail = kv;
	} else {
		/* head also NULL and both next are NULL */
		this->kvsHead = kv;
		this->kvsTail = kv;
	}

	++this->count;

	return true;

}


public: kvo_t<TK, TV> remove (TK key) {

	SimpleKV::KV * tkv = this->kvsHead;

	kvo_t<TK, TV> kvo;

	while (NULL != tkv) {
		if (this->equ(key, tkv->kv.key)) {
			/* found */
			kvo.set = true;
			kvo.key = tkv->kv.key;
			kvo.value = tkv->kv.value;

			this->kvsHead = tkv->next;

			if (NULL == this->kvsHead) {
				/* no more also set tail */
				this->kvsTail = NULL;
			}
			delete tkv;
			tkv = NULL;

			--count;
			return kvo;
		}

		tkv = tkv->next;
	}

	kvo.set = false;
	return kvo;

}


public: int set(TK key, TV value);
public: SimpleKV::kv_t get(int i);

public: bool hasKey (TK key) {

	SimpleKV::KV * tkv = this->kvsHead;

	while (NULL != tkv) {
		if (this->equ(key, tkv->kv.key)) {
			return true;
		} else {
			tkv = tkv->next;
		}
	}

	return false;

}


public: void empty (void) {

	SimpleKV::KV * tkv = this->kvsHead;
	SimpleKV::KV * next;

	while (NULL != tkv) {
		next = tkv->next;
		tkv->next = NULL;

		delete tkv;
		tkv = NULL;

		tkv = next;
	}

	count = 0;
	this->kvsHead = NULL;
	this->kvsTail = NULL;

}


private: SimpleKV::KV * kvsHead;
private: SimpleKV::KV * kvsTail;
private: SimpleKV::equ_t * equ;
private: int count;

};


template<class TK, class TV>
class SimpleKVMaker
{

protected: typedef bool (equ_t)(TK key1, TK key2);

public: static SimpleKV<TK, TV> * make (SimpleKVMaker::equ_t * equ)
{
	if (NULL == equ) {
		return NULL;
	} else {
		return new SimpleKV<TK, TV>(equ);
	}
}

};


#endif /* !defined(SIMPLEKV_HXX__) */
