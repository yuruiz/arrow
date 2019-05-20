/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.apache.arrow.adapter.orc;

import java.io.IOException;

import org.apache.arrow.memory.BufferAllocator;
import org.apache.arrow.vector.ipc.ArrowReader;


public class OrcReader implements AutoCloseable {

  private static final int DEFAULT_BATCH_SIZE = 1024;

  private OrcReaderJniWrapper orcReader;

  private BufferAllocator allocator;

  public OrcReader(String filePath, BufferAllocator allocator) throws IOException {
    this.allocator = allocator;
    orcReader = new OrcReaderJniWrapper();
    orcReader.open(filePath);
  }

  public ArrowReader nextStripeReader() {
    OrcStripeReaderJniWrapper stripeReader = orcReader.nextStripeReader(DEFAULT_BATCH_SIZE);
    if (stripeReader == null) {
      return null;
    }

    return new OrcStripeReader(stripeReader, allocator);
  }

  @Override
  public void close() throws Exception {
    orcReader.close();
  }
}
