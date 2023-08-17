# BufferQueue机制

## 1. 类的关系

### 1.1 BufferQueue

#### 1.1.1. 引入

`BufferQueue` 的原始生产者和消费者分别为 `BufferQueueProducer` 和 `BufferQueueConsumer`，其关系如下图所示。

![bufferqueue原理](picture/bufferqueue%E5%8E%9F%E7%90%86.png)

##### BufferQueueProducer
生产者通过 `RequestBuffer()` 从 `BufferQueue` 中请求一个可用的 buffer，
绘制完毕后调用 `FlushBuffer()` 送显，并回调消费者监听回调函数 `OnBufferAvailable()`。

##### BufferQueueConsumer 
消费者收到通知后调用 `AcquireBuffer()` 获取 buffer，
使用完后调用 `ReleaseBuffer()` 释放，并回调生产者监听回调函数 `OnBufferRelease()`。

##### BufferQueue
+ 申请内存：当 `BufferQueue` 中的队列中没有可重复使用的 buffer 时，会调用 `AllocBuffer()` 申请内存，会通过创建 `SurfaceBufferImpl` 申请内存，并赋给新的 `BufferElement`。
`SurfaceBufferImpl` 是抽象类 `SurfaceBuffer` 的实现类。

+ 释放内存：调用 `DeleteBuffers`，将 `BufferQueue` 中的 `freeList_` 和 `dirtyList_` 中的元素清空，将 `bufferQueueCache_` 中所映射到的 `BufferElement` 对象的 `isDeleting` 置 `True`。

##### Buffer 状态
同时，buffer 的状态随生产者、消费者的操作而改变，其状态转移关系如下图所示。

![buffer状态图](picture/buffer%E7%8A%B6%E6%80%81%E5%9B%BE.png)


#### 1.1.2. BufferQueue 相关的类

`BufferQueueProducer` 和 `BufferQueueConsumer` 均包含一个指向 `BufferQueue` 对象的指针，通过 `BufferQueue` 对象操作内存。