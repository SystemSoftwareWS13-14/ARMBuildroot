# Design - buf_threaded

Das Modul buf_threaded simuliert mithilfe eines asynchronen threadsicheren Buffers externe Hardware.
Der read/write Zugriff auf diesen Buffer soll hierbei in kernel Threads erfolgen.

## Inhalt
* Buffer
* Modul
* Datenfluss
* Kritische Bereiche
* Kritik

## Buffer
Der Buffer wurde in eine seperaten Date implementiert. Im unter4schied zum Buffer aus der Aufgabe
"Zugrissmodi" verwendet dieser einen Mutex, um den gleichzeitigen Zugriff durch mehrere Threads zu verhindern.
Dadurch wird immer die Konsistenz des Buffers sichergestellt.

```
mutex_lock(&buf->buffer_mutex);
toRead = min(byte, buf->byteCount);
for (i = 0; i < toRead; ++i) {
        out[i] = buf->data[buf->index];
        buf->index = (buf->index + 1) % buf->size;
}
buf->byteCount -= toRead;
mutex_unlock(&buf->buffer_mutex);
```

## Modul
Im kernel Modul buf_threaded sind die read und write Threads implementiert.

## Datenfluss
Bei read oder write Zugriffen auf die dazugehörige device-File werden die Funktionen **read** bzw. **write** aufgerufen.

## Kritische Bereiche
