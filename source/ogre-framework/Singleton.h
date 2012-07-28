// Macros for using Ogre's Singleton approach in just 2 lines

#include <OgreSingleton.h>

#if OGRE_VERSION_MINOR < 8
	#define SINGLETON_MEMBER ms_Singleton
#else
	#define SINGLETON_MEMBER msSingleton
#endif

#define DECLARE_SINGLETON(classname) \
	static classname& getSingleton(void); \
	static classname* getSingletonPtr(void);
    
#define IMPLEMENT_SINGLETON(classname) \
	template<> classname* Ogre::Singleton<classname>::SINGLETON_MEMBER = 0; \
	classname* classname::getSingletonPtr(void) \
	{ \
		return SINGLETON_MEMBER; \
	} \
	classname& classname::getSingleton(void) \
	{ \
		assert( SINGLETON_MEMBER );  return ( *SINGLETON_MEMBER ); \
	} \

