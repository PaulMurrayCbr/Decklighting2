const { useState, useEffect, useRef, useCallback } = React;

function DurationSlider({ label, value, setValue, min, max, api }) {

	const SLIDER_RESOLUTION = 10000;

	const minLog = useRef(0);
	const maxLog = useRef(Math.log(10));
	const s2vScale = useRef(1);
	const v2sScale = useRef(1);

	const [slider, setSlider] = useState(0);

	const apiRef = useRef(api);
	useEffect(() => {
		apiRef.current = api;
	}, [api]);

	function apiStable(value) {
		apiRef.current(value);
	}
	
	
	function clamp(value, min, max) {
		return Math.min(Math.max(Math.round(value), min), max);
	}

	function sliderToValue(pos) {
		return clamp(
			Math.exp(minLog.current + s2vScale.current * pos),
			min, max);
	}

	function valueToSlider(val) {
		return clamp((Math.log(val) - minLog.current) * v2sScale.current,
			0, SLIDER_RESOLUTION);
	}

	const debouncerCallback = useCallback(v => {
		apiStable(sliderToValue(v));
	});

	const debouncer = useRef(new Debouncer(100, setSlider, debouncerCallback));


	function threeDigits(n) {
		if (Math.abs(n) > 100)
			return "" + Math.round(n);
		else if (Math.abs(n) > 10)
			return "" + (Math.round(n * 10) / 10);
		else if (Math.abs(n) > 1)
			return "" + (Math.round(n * 100) / 100);
		else
			return "" + (Math.round(n * 1000) / 1000);
	}

	const timeUnits = [
		{ u: 'd', t: 24 * 60 * 60 * 1000 },
		{ u: 'h', t: 60 * 60 * 1000 },
		{ u: 'm', t: 60 * 1000 }
	];

	function timeString(t) {
		if (Math.abs(t) < 1000)
			return threeDigits(1000 / t) + ' fps'

		let s = '';

		let used = 0;
		let remaining = t;
		for (const tu of timeUnits) {
			if (tu.t > t) continue;
			if (used > 0) {
				s += ' ';
			}
			const amt = Math.floor(remaining / tu.t);

			s += amt + tu.u;
			remaining -= amt * tu.t;
			if (++used >= 2) break;

		}
		if (used == 0) {
			// don't need ms, because we will get fps instead.
			s += threeDigits(remaining / 1000) + 's'
		}
		else if (used == 1) {
			s += ' ';
			s += Math.floor(remaining / 1000) + 's'
		}

		return s;
	}


	useEffect(() => {
		minLog.current = Math.log(min);
		maxLog.current = Math.log(max);
		s2vScale.current = (maxLog.current - minLog.current) / SLIDER_RESOLUTION;
		v2sScale.current = SLIDER_RESOLUTION / (maxLog.current - minLog.current);
	}, [min, max]);

	useEffect(() => {
		setSlider(valueToSlider(value));
	}, [value]);

	return (

		<div className="row mb-3">
			<label className="col-sm-3 col-form-label">{label}</label>
			<div className="col-sm-9 d-flex align-items-center">
				<input
					type="range"
					className="form-range"
					min="0"
					max={SLIDER_RESOLUTION}
					value={slider}
					onChange={e => debouncer.current.set(e.target.value)}
					style={{ flexGrow: 1 }}  // slider fills available space
				/>
				<span className="ms-3" style={{ whiteSpace: 'nowrap' }}>
					{timeString(sliderToValue(slider))}
				</span>
			</div>
		</div>

	);



}
