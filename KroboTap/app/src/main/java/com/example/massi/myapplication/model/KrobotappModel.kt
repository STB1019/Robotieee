package com.example.massi.myapplication.model

import com.example.massi.myapplication.KrobotAppEvent
import io.reactivex.Observable
import io.reactivex.subjects.PublishSubject
import io.reactivex.subjects.Subject

class KrobotappModel() {

    lateinit var explorationMap : ExplorationGrid
    lateinit var sokobanMap : SokobanGrid

    private var _appEventStream : PublishSubject<KrobotAppEvent> = PublishSubject.create()
    public val readStream : Observable<KrobotAppEvent> = _appEventStream
    public val writeStream : Subject<KrobotAppEvent> = _appEventStream

}