package server

import (
	"sync"
	"time"
)

//go:generate stringer -type=ValueType --trimprefix ValueType
type ValueType int

const (
	ValueTypeNone ValueType = iota
	ValueTypeString
)

type Value struct {
	Type    ValueType
	V       string
	expired int64
}

type Server struct {
	data   map[string]Value
	dataMu sync.RWMutex
}

func NewServer() *Server {
	return &Server{
		data: map[string]Value{},
	}
}

func (s *Server) setWithExpired(k string, v string, expireDur time.Duration) {
	expiredTimestamp := time.Now().Add(expireDur).UnixNano()
	if expireDur == -1 {
		expiredTimestamp = -1
	}
	s.dataMu.Lock()
	s.data[k] = Value{
		Type:    ValueTypeString,
		V:       v,
		expired: expiredTimestamp,
	}
	s.dataMu.Unlock()
}

func (s *Server) get(k string) (string, bool) {
	currentTimestamp := time.Now().UnixNano()
	s.dataMu.RLock()
	v, ok := s.data[k]
	s.dataMu.RUnlock()
	if !ok {
		return "", false
	}
	if v.expired < currentTimestamp && v.expired != -1 {
		return "", false
	}
	return v.V, true
}

func (s *Server) getType(k string) ValueType {
	currentTimestamp := time.Now().UnixNano()
	s.dataMu.RLock()
	v, ok := s.data[k]
	s.dataMu.RUnlock()
	if !ok {
		return ValueTypeNone
	}
	if v.expired < currentTimestamp && v.expired != -1 {
		return ValueTypeNone
	}
	return v.Type
}
