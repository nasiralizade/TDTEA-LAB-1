// del 1: Användning av Closures för iteratörer.
function ArrayIterator(arr) {
    let currentIndex = 0;
    this.hasNext = function () {
        return currentIndex < arr.length;
    }
    this.next = function () {
        if (this.hasNext()) {
            return arr[currentIndex++];
        } else {
            throw new Error('No more elements in iterator');
        }
    }
}

// Test
const arr = [1, 2, 3, 4, 5];
const iterator = new ArrayIterator(arr);
while (iterator.hasNext()) {
    console.log(iterator.next());
}

// del 2: Användning av Prototypmekanismen
Array.prototype.iterator = function () {
    let currentIndex = 0;
    const arr = this;
    return {
        hasNext: function () {
            return currentIndex < arr.length;
        },
        next: function () {
            if (this.hasNext()) {
                return arr[currentIndex++];
            } else {
                throw new Error('No more elements in iterator');
            }
        }
    }
}
// Test
const arr2 = [1, 2, 3, 4, 5];
const iterator2 = arr2.iterator();
while (iterator2.hasNext()) {
    console.log(iterator2.next());
}