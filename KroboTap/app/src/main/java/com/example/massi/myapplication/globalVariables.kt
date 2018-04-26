package com.example.massi.myapplication

import com.example.massi.myapplication.model.KrobotappModel
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject
import java.lang.ref.WeakReference
import java.util.concurrent.locks.Lock
import java.util.concurrent.locks.ReentrantLock
import kotlin.concurrent.withLock

/**
 * Singleton class containing the variables implementing the single reactive event bus
 *
 * Instad of using them everywhere, it is strongly encouraged to create extension methods or something
 * to mask thri access: in this way you can generalize all your business logic
 */
public class GlobalVariables {

    companion object {

        /**
         * The model of the application
         */
        public val model : KrobotappModel = KrobotappModel()

    }

}