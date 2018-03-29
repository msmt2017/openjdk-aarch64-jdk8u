
/*
 * Copyright (c) 2016, Red Hat, Inc. and/or its affiliates.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 *
 */

#ifndef SHARE_VM_GC_SHENANDOAH_SHENANDOAHFREESET_HPP
#define SHARE_VM_GC_SHENANDOAH_SHENANDOAHFREESET_HPP

#include "gc_implementation/shenandoah/shenandoahHeap.hpp"
#include "gc_implementation/shenandoah/shenandoahHeapRegionSet.hpp"

class ShenandoahFreeSet : public CHeapObj<mtGC> {
private:
  ShenandoahHeap* const _heap;
  ShenandoahHeapRegionSet* _regions;
  BitMap _mutator_free_bitmap;
  BitMap _collector_free_bitmap;
  size_t _max;

  // Left-most and right-most region indexes. There are no free regions outside
  // of [left-most; right-most] index intervals
  size_t _mutator_leftmost, _mutator_rightmost;
  size_t _collector_leftmost, _collector_rightmost;

  size_t _capacity;
  size_t _used;

  void assert_bounds() const PRODUCT_RETURN;
  void assert_heaplock_owned_by_current_thread() const PRODUCT_RETURN;
  void assert_heaplock_not_owned_by_current_thread() const PRODUCT_RETURN;

  bool is_mutator_free(size_t idx) const;
  bool is_collector_free(size_t idx) const;

  HeapWord* try_allocate_in(ShenandoahHeapRegion* region, size_t word_size, ShenandoahHeap::AllocType type, bool& in_new_region);
  HeapWord* allocate_single(size_t word_size, ShenandoahHeap::AllocType type, bool& in_new_region);
  HeapWord* allocate_contiguous(size_t words_size);

  void flip_to_mutator(size_t idx);
  void flip_to_gc(size_t idx);

  void adjust_bounds();
  bool touches_bounds(size_t num) const;

  void increase_used(size_t amount);
  void clear_internal();

  size_t collector_count() const { return _collector_free_bitmap.count_one_bits(); }
  size_t mutator_count()   const { return _mutator_free_bitmap.count_one_bits();   }

  void try_recycle_trashed(ShenandoahHeapRegion *r);

  bool is_empty_or_trash(ShenandoahHeapRegion *r);
  size_t alloc_capacity(ShenandoahHeapRegion *r);
  bool has_no_alloc_capacity(ShenandoahHeapRegion *r);

public:
  ShenandoahFreeSet(ShenandoahHeap* heap, ShenandoahHeapRegionSet* regions, size_t max_regions);

  void clear();
  void rebuild();

  void recycle_trash();

  size_t capacity()  const { return _capacity; }
  size_t used()      const { return _used;     }
  size_t available() const {
    assert(_used <= _capacity, "must use less than capacity");
    return _capacity - _used;
  }

  HeapWord* allocate(size_t words_size, ShenandoahHeap::AllocType type, bool &in_new_region);
  size_t unsafe_peek_free() const;
  void print_on(outputStream* out) const;
};

#endif //SHARE_VM_GC_SHENANDOAH_SHENANDOAHFREESET_HPP
