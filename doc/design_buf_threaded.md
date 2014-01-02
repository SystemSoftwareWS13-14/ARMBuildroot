# Design - buf_threaded

Das Modul buf_threaded simuliert mithilfe eines asynchronen threadsicheren Buffers externe Hardware.
Der read/write Zugriff auf diesen (FIFO)-Buffer soll hierbei in kernel Threads erfolgen. In dieser Implementierung
werden direkt eigene Threads verwendet (keine work_queues oder sonstiges).

## Inhalt
1. Buffer
2. Modul
 1. Threads
3. Datenfluss
4. Synchronisierung
 1. Buffer voll
 2. Buffer leer
 3. Read/write-Zugriff
5. Kritik

## Buffer
Der Buffer wurde in einer separaten Datei implementiert. Im Unterschied zum Buffer aus der Aufgabe
"Zugrifsmodi" verwendet dieser einen Mutex, um den gleichzeitigen Zugriff durch mehrere Threads zu verhindern.
Dadurch wird immer die Konsistenz des Buffers sichergestellt.

Der folgende Codeausschnitt zeigt die read funktion:
```C
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
Im kernel Modul buf_threaded sind die read und write Threads implementiert. Jeder read oder write Zugriff
greift auf den gemeinsamen globalen Buffer zu. Zur Synchronisierung mehrere gleichzeitiger Zugriffe auf den
Buffer sowie zum Synchronisieren der Zustände Buffer-voll und Buffer-leer wird ein globaler **Mutex** verwendet.
Für blockierende read/write Zugriffe werden zusätzlich zwei **wait_queues** benutzt.

```C
static buffer dev_buf;
static wait_queue_head_t read_wait_queue;
static wait_queue_head_t write_wait_queue;
DEFINE_MUTEX(mutex)
```

### Threads
Die jeweiligen read und write Threads bekommen als parameter beim Start jeweils eine Datenstruktur vom Typ **thread_data_t** übergeben.

```C
typedef struct {
        char *r_buff; // Read
        const char __user *w_buff; // Write
        size_t size;
        struct completion *compl;
} thread_data_t;
```

Diese Struktur enthält Pointer auf die zu lesende/schreibende Buffer sowie deren Größe. Die Struktur **struct completion** wird verwendet, um dem erzeugenden Thread das Ende diesen Threads zu Signalisieren.
Aufgabe der Threads ist es, mithilfe eines zufälligen Sleeps die langsamere Hardware zu simulieren und anschließend
in den Buffer zu schreiben/von dem Buffer zu lesen. Danach wird die Completion-Variable **compl** gesetzt und auf
ein kthread_stop des erzeugenden Threads gewartet. Nach Rückgabe des Rückgabewertes ist der Thread beendet.

## Datenfluss
Bei read oder write Zugriffen auf die dazugehörige device-File werden die Funktionen **read** bzw. **write** aufgerufen.

## Synchronisierung

Da die read und write Funktionen von der synchronisierung nahezu identisch sind, werden hier die Synchronisierungsmaßnahmen lediglich allgemein erklärt.
