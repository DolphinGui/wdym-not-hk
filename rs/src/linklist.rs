use std::{
    default,
    ops::DerefMut,
    ptr::{null, null_mut},
};

struct Node<T> {
    item: T,
    next: Option<Box<Node<T>>>,
    prev: *mut Node<T>,
}

impl<T: std::default::Default> Default for Node<T> {
    fn default() -> Self {
        Self {
            item: Default::default(),
            next: Default::default(),
            prev: null_mut(),
        }
    }
}

impl <T> Node<T> {
    fn link_next(& mut self, mut next: Box<Node<T>>){
        next.prev = self;
        match &self.next {
            Some(old) => todo!(),
            None => {self.next = Some(next)},
        }
    }
}

pub struct LinkedList<T> {
    head: Option<Box<Node<T>>>,
    tail: *mut Node<T>,
}

impl<T: std::default::Default> Default for LinkedList<T> {
    fn default() -> Self {
        Self {
            head: None,
            tail: null_mut(),
        }
    }
}

impl<T: std::default::Default> LinkedList<T> {
    pub fn new() -> Self {
        Self::default()
    }

    fn push_front_node(&mut self, new: Box<Node<T>>){
        match &self.head{
            Some(old) => {
                self.head = Some(new)
            },
            None => self.head = Some(new),
        }
    }
}
