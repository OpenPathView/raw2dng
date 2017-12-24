#ifndef __LISTOF_H
#define __LISTOF_H

template <class T>
class listof
{
   int count_;
   T* first_;
   T* last_;

public:
   listof() : count_(0), first_(0), last_(0) {}
   ~listof() { free(); }

   void free();
   T* add(T* t);
   T* add_head(T* t);
   T* detach(T* t);

   void remove(T* t) { delete detach(t); }
   operator T* () const { return first_; }

   int count() const { return count_; }
};

template <class T>
void listof<T>::free()
{
   T* curr = first_;
   while( curr )
   {
      T* temp = curr;
      curr = curr->next;
      delete temp;
   }
   count_ = 0;
   first_ = 0;
   last_ = 0;
}

template <class T>
T* listof<T>::detach(T* obj)
{
   if( obj == first_ )
   {
      first_ = obj->next;
      obj->next = 0;

      if( last_ == obj )
         last_ = 0;

      --count_;
      return obj;
   }
   else
   {
      T* curr = first_;
      while( curr )
      {
         if( curr->next == obj )
         {
            curr->next = obj->next;
            obj->next = 0;
            
            if( last_ == obj )
               last_ = curr;
               
            --count_;
            return obj;
         }
         curr = curr->next;
      }
   }

   return 0;
}

template <class T>
T* listof<T>::add(T* obj)
{
   if( !first_ )
   {
      first_ = obj;
   }
   else
   {
      last_->next = obj;
   }
   obj->next = 0;
   last_ = obj;
   ++count_;
   return obj;
}

template <class T>
T* listof<T>::add_head(T* obj)
{
   obj->next = first_;
   first_ = obj;
   
   if( !last_ )
   {
      last_ = obj;
   }

   ++count_;
   return obj;
}

#endif